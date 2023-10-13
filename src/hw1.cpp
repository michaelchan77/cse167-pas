#include "hw1.h"
#include "hw1_scenes.h"

using namespace hw1;

struct check_shape_op {
    Real x, y;
    bool operator()(const Circle &circle) const {
        return length(Vector2{x, y} - circle.center) < circle.radius;
    }
    bool operator()(const Rectangle &rectangle) const {
        return x > rectangle.p_min.x && x < rectangle.p_max.x &&
               y > rectangle.p_min.y && y < rectangle.p_max.y;
    }
    bool operator()(const Triangle &triangle) const { return false; }
};

struct rasterize_shape_op {
    Image3 &img;
    const Real SSAA = 4;
    // constructor
    rasterize_shape_op(Image3 &image) : img(image) {}
    // bounding box helper
    std::pair<Vector2,Vector2> bBoxTransform(const Shape &shape, Vector2 pMin, Vector2 pMax) {
        // transform coordinates
        Vector3 UL = get_transform(shape) * Vector3{pMin.x, pMin.y, Real(1)};
        Vector3 UR = get_transform(shape) * Vector3{pMax.x, pMin.y, Real(1)};
        Vector3 LL = get_transform(shape) * Vector3{pMin.x, pMax.y, Real(1)};
        Vector3 LR = get_transform(shape) * Vector3{pMax.x, pMax.y, Real(1)};
        // new bounding box
        Vector2 pMinT = Vector2{std::min({UL.x, UR.x, LL.x, LR.x}),
                                std::min({UL.y, UR.y, LL.y, LR.y})};
        Vector2 pMaxT = Vector2{std::max({UL.x, UR.x, LL.x, LR.x}),
                                std::max({UL.y, UR.y, LL.y, LR.y})};                          
        // restrict to screen
        Vector2 min_bound = Vector2{std::max(pMinT.x, Real(0)), 
                                    std::max(pMinT.y, Real(0))};
        Vector2 max_bound = Vector2{std::min(pMaxT.x, Real(img.width)),
                                    std::min(pMaxT.y, Real(img.height))};
        return std::pair(min_bound, max_bound);
    }
    // antialiasing helper
    Vector3 getAvgColor(const Shape &shape, int x, int y) {
        Real r = 0, g = 0, b = 0;
        Real subPixCenter = Real(1)/(2 * SSAA), nextSubPix = 2 * subPixCenter;
        for (Real yi = subPixCenter; yi < Real(1); yi += nextSubPix) {
            for (Real xi = subPixCenter; xi < Real(1); xi += nextSubPix) {
                Vector3 p = Vector3{x + xi, y + yi, Real(1)}; 
                Vector3 pT = inverse(get_transform(shape)) * p;
                Vector3 background = img(p.x,p.y);
                if(std::visit(check_shape_op{pT.x,pT.y},shape)) {
                    // alpha blending
                    Vector3 color = get_alpha(shape) * get_color(shape) + 
                                    (1 - get_alpha(shape)) * background;
                    r += color.x, g += color.y, b += color.z;
                } else {
                    r += background.x, g += background.y, b += background.z;
                }
            }
        }
        return Vector3{r/(SSAA*SSAA), g/(SSAA*SSAA), b/(SSAA*SSAA)};
    }
    // rasterize circle
    void operator()(const Circle &circle) {
        Vector2 pMin = Vector2{circle.center.x - circle.radius, 
                               circle.center.y - circle.radius};
        Vector2 pMax = Vector2{circle.center.x + circle.radius, 
                               circle.center.y + circle.radius};
        std::pair bounds = bBoxTransform(circle, pMin, pMax);
        Vector2 min_bound = bounds.first, max_bound = bounds.second;

        for (int y = min_bound.y; y < max_bound.y; y++) {
            for (int x = min_bound.x; x < max_bound.x; x++) {
                img(x,y) = getAvgColor(circle, x, y);
            }
        }
    }
    // rasterize rectangle
    void operator()(const Rectangle &rectangle) {
        // bounding box
        std::pair bounds = bBoxTransform(rectangle, rectangle.p_min, rectangle.p_max);
        Vector2 min_bound = bounds.first, max_bound = bounds.second;

        for (int y = min_bound.y; y < max_bound.y; y++) {
            for (int x = min_bound.x; x < max_bound.x; x++) {
                img(x,y) = getAvgColor(rectangle, x, y);
            }
        }
    }
    void operator()(const Triangle &triangle) {
        // bounding box
        Vector2 pMin = Vector2{std::min({triangle.p0.x, triangle.p1.x, triangle.p2.x}), 
                               std::min({triangle.p0.y, triangle.p1.y, triangle.p2.y})};
        Vector2 pMax = Vector2{std::max({triangle.p0.x, triangle.p1.x, triangle.p2.x}), 
                               std::max({triangle.p0.y, triangle.p1.y, triangle.p2.y})}; 
        std::pair bounds = bBoxTransform(triangle, pMin, pMax);
        Vector2 min_bound = bounds.first, max_bound = bounds.second;
        // edges and normals
        Vector2 e01 = triangle.p1 - triangle.p0, n01 = Vector2{e01.y, -e01.x};
        Vector2 e12 = triangle.p2 - triangle.p1, n12 = Vector2{e12.y, -e12.x};
        Vector2 e20 = triangle.p0 - triangle.p2, n20 = Vector2{e20.y, -e20.x};
        // rasterize
        for (int y = min_bound.y; y < max_bound.y; y++) {
            for (int x = min_bound.x; x < max_bound.x; x++) {
                // SSAA
                Real r = 0, g = 0, b = 0;
                Real subPixCenter = Real(1)/(2*SSAA), nextSubPix = 2*subPixCenter;
                for (Real yi = subPixCenter; yi < Real(1); yi += nextSubPix) {
                    for (Real xi = subPixCenter; xi < Real(1); xi += nextSubPix) {
                        Vector3 p = Vector3{x + xi, y + yi, Real(1)}; 
                        Vector3 pT = inverse(triangle.transform) * p;
                        Vector2 q01 = Vector2{pT.x, pT.y} - triangle.p0;
                        Vector2 q12 = Vector2{pT.x, pT.y} - triangle.p1;
                        Vector2 q20 = Vector2{pT.x, pT.y} - triangle.p2;
                        Vector3 background = img(p.x,p.y);
                        bool isPos = dot(q01,n01) > 0 && dot(q12,n12) > 0 && dot(q20,n20) > 0;
                        bool isNeg = dot(q01,n01) < 0 && dot(q12,n12) < 0 && dot(q20,n20) < 0;
                        if (isPos || isNeg) {
                            // alpha blending
                            Vector3 color = triangle.alpha*triangle.color + 
                                            (1-triangle.alpha)*background;
                            r += color.x, g += color.y, b += color.z;
                        } else {
                            r += background.x, g += background.y, b += background.z;
                        }
                    }
                }
                img(x,y) = Vector3{r/(SSAA*SSAA), g/(SSAA*SSAA), b/(SSAA*SSAA)};
            }
        }
    }
}; 

Image3 hw_helper(const std::vector<std::string> &params) {
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);
    // initialize background
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background;
        }
    }
    // rasterize
    for (const auto &shape : scene.shapes) {
        std::visit(rasterize_shape_op(img), shape);
    }
    return img;
}

Image3 hw_1_1(const std::vector<std::string> &params) {
    // Homework 1.1: render a circle at the specified
    // position, with the specified radius and color.

    Image3 img(640 /* width */, 480 /* height */);

    Vector2 center = Vector2{img.width / 2 + Real(0.5), img.height / 2 + Real(0.5)};
    Real radius = 100.0;
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    // get params
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-center") {
            Real x = std::stof(params[++i]);
            Real y = std::stof(params[++i]);
            center = Vector2{x, y};
        } else if (params[i] == "-radius") {
            radius = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        }
    }
    // rasterize
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector2 curr = Vector2{x + Real(0.5), y + Real(0.5)};
            if(length(curr - center) < radius) {
                img(x, y) = color;
            } else {
                img(x, y) = Vector3{0.5, 0.5, 0.5};
            }
        }
    }
    return img;
}

Image3 hw_1_2(const std::vector<std::string> &params) {
    // Homework 1.2: render multiple circles
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    int scene_id = std::stoi(params[0]);
    const CircleScene &scene = hw1_2_scenes[scene_id];

    Image3 img(scene.resolution.x, scene.resolution.y);
    // initialize background
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background;
        }
    }
    // rasterization
    for (Circle c : scene.objects) {
        // bounding box
        Vector2 UL = Vector2{std::max(c.center.x - c.radius, Real(0.)), 
                             std::max(c.center.y - c.radius, Real(0.))};
        Vector2 LR = Vector2{std::min(c.center.x + c.radius, Real(img.width)), 
                             std::min(c.center.y + c.radius, Real(img.height))};
        for (int y = UL.y; y < LR.y; y++) {
            for (int x = UL.x; x < LR.x; x++) {
                Vector2 curr = Vector2{x + Real(0.5), y + Real(0.5)};
                if(length(curr - c.center) < c.radius)
                    img(x, y) = c.color;
            }
        }
    }
    return img;
}

Image3 hw_1_3(const std::vector<std::string> &params) {
    // Homework 1.3: render multiple shapes
    return hw_helper(params);
}

Image3 hw_1_4(const std::vector<std::string> &params) {
    // Homework 1.4: render transformed shapes
    return hw_helper(params);
}

Image3 hw_1_5(const std::vector<std::string> &params) {
    // Homework 1.5: antialiasing
    return hw_helper(params);
}

Image3 hw_1_6(const std::vector<std::string> &params) {
    // Homework 1.6: alpha blending
    return hw_helper(params);
}
