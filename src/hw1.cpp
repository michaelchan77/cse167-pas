#include "hw1.h"
#include "hw1_scenes.h"

using namespace hw1;

struct rasterize_shape_op {
    // Credit: Function skeleton from @bwu on Discord 
    // (CSE167 FA23 server, #homework-help channel)

    Image3 &img;
    // constructor
    rasterize_shape_op(Image3 &image) : img(image) {
    }

    // bounding box helper
    std::pair<Vector2,Vector2> boundingBoxTransform(const Shape &shape, Vector2 pMin, Vector2 pMax) {
        // transform coordinates
        Vector3 upLeftT = get_transform(shape)*Vector3{pMin.x, pMin.y, Real(1)};
        Vector3 upRightT = get_transform(shape)*Vector3{pMax.x, pMin.y, Real(1)};
        Vector3 lowLeftT = get_transform(shape)*Vector3{pMin.x, pMax.y, Real(1)};
        Vector3 lowRightT = get_transform(shape)*Vector3{pMax.x, pMax.y, Real(1)};
        // new bounding box
        Vector2 pMinT = Vector2{std::min({upLeftT.x,upRightT.x,lowLeftT.x,lowRightT.x}),
                                  std::min({upLeftT.y,upRightT.y,lowLeftT.y,lowRightT.y})};
        Vector2 pMaxT = Vector2{std::max({upLeftT.x,upRightT.x,lowLeftT.x,lowRightT.x}),
                                  std::max({upLeftT.y,upRightT.y,lowLeftT.y,lowRightT.y})};                          
        // restrict to screen
        Vector2 min_bound = Vector2{std::max(pMinT.x, Real(0)), 
                                    std::max(pMinT.y, Real(0))};
        Vector2 max_bound = Vector2{std::min(pMaxT.x, Real(img.width)),
                                    std::min(pMaxT.y, Real(img.height))};
        return std::pair(min_bound, max_bound);
    }
    
    void operator()(const Circle &circle) {
        // bounding box
        Vector2 pMin = Vector2{circle.center.x-circle.radius, circle.center.y-circle.radius};
        Vector2 pMax = Vector2{circle.center.x+circle.radius, circle.center.y+circle.radius};
        std::pair bounds = boundingBoxTransform(circle, pMin, pMax);
        Vector2 min_bound = bounds.first;
        Vector2 max_bound = bounds.second;
        // rasterize
        for (int y = min_bound.y; y < max_bound.y; y++) {
            for (int x = min_bound.x; x < max_bound.x; x++) {
                Vector3 p = Vector3{x + Real(0.5), y + Real(0.5), Real(1)};
                Vector3 pT = inverse(circle.transform) * p;
                if(length(Vector2{pT.x, pT.y} - circle.center) < circle.radius)
                    img(x, y) = circle.color;
            }
        }
    }
    void operator()(const Rectangle &rectangle) {
        // bounding box
        std::pair bounds = boundingBoxTransform(rectangle, rectangle.p_min, rectangle.p_max);
        Vector2 min_bound = bounds.first;
        Vector2 max_bound = bounds.second;
        // rasterize
        for (int y = min_bound.y; y < max_bound.y; y++) {
            for (int x = min_bound.x; x < max_bound.x; x++) {
                Vector3 p = Vector3{x + Real(0.5), y + Real(0.5), Real(1)};
                Vector3 pT = inverse(rectangle.transform) * p;
                if (pT.x > rectangle.p_min.x && pT.x < rectangle.p_max.x &&
                    pT.y > rectangle.p_min.y && pT.y < rectangle.p_max.y)
                    img(x, y) = rectangle.color;
            }
        }
    }
    void operator()(const Triangle &triangle) {
        // bounding box
        Vector2 pMin = Vector2{std::min({triangle.p0.x, triangle.p1.x, triangle.p2.x}), 
                               std::min({triangle.p0.y, triangle.p1.y, triangle.p2.y})};
        Vector2 pMax = Vector2{std::max({triangle.p0.x, triangle.p1.x, triangle.p2.x}), 
                               std::max({triangle.p0.y, triangle.p1.y, triangle.p2.y})}; 
        std::pair bounds = boundingBoxTransform(triangle, pMin, pMax);
        Vector2 min_bound = bounds.first;
        Vector2 max_bound = bounds.second;
        // define normal vectors
        Vector2 e01 = triangle.p1 - triangle.p0;
        Vector2 n01 = Vector2{e01.y, -e01.x};
        Vector2 e12 = triangle.p2 - triangle.p1;
        Vector2 n12 = Vector2{e12.y, -e12.x};
        Vector2 e20 = triangle.p0 - triangle.p2;
        Vector2 n20 = Vector2{e20.y, -e20.x};
        // rasterize
        for (int y = min_bound.y; y < max_bound.y; y++) {
            for (int x = min_bound.x; x < max_bound.x; x++) {
                Vector3 p = Vector3{x + Real(0.5), y + Real(0.5), Real(1)};
                Vector3 pT = inverse(triangle.transform) * p;
                Vector2 q01 = Vector2{pT.x, pT.y} - triangle.p0;
                Vector2 q12 = Vector2{pT.x, pT.y} - triangle.p1;
                Vector2 q20 = Vector2{pT.x, pT.y} - triangle.p2;
                bool isPos = dot(q01,n01) > 0 && dot(q12,n12) > 0 && dot(q20,n20) > 0;
                bool isNeg = dot(q01,n01) < 0 && dot(q12,n12) < 0 && dot(q20,n20) < 0;
                if (isPos || isNeg) {
                    img(x, y) = triangle.color;
                }
            }
        }
    }
}; 

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
    // rasterize
    for (const Shape &shape : scene.objects) {
        std::visit(rasterize_shape_op(img), shape);
    }
    return img;
}

Image3 hw_1_3(const std::vector<std::string> &params) {
    // Homework 1.3: render multiple shapes
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

Image3 hw_1_4(const std::vector<std::string> &params) {
    // Homework 1.4: render transformed shapes
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

Image3 hw_1_5(const std::vector<std::string> &params) {
    // Homework 1.5: antialiasing
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}

Image3 hw_1_6(const std::vector<std::string> &params) {
    // Homework 1.6: alpha blending
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}
