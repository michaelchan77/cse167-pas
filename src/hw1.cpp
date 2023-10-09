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
    void operator()(const Circle &circle) {
        // bounding box
        Real minX = std::max(circle.center.x - circle.radius, Real(0.));
        Real minY = std::max(circle.center.y - circle.radius, Real(0.));
        Real maxX = std::min(circle.center.x + circle.radius, Real(img.width));
        Real maxY = std::min(circle.center.y + circle.radius, Real(img.height));
        Vector2 pMin = Vector2{minX, minY};
        Vector2 pMax = Vector2{maxX, maxY};
        // rasterize
        for (int y = pMin.y; y < pMax.y; y++) {
            for (int x = pMin.x; x < pMax.x; x++) {
                Vector2 p = Vector2{x + Real(0.5), y + Real(0.5)};
                if(length(p - circle.center) < circle.radius)
                    img(x, y) = circle.color;
            }
        }
    }
    void operator()(const Rectangle &rectangle) {
        // ADD
    }
    void operator()(const Triangle &triangle) {
        // ADD
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

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
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
