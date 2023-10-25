#include "hw2.h"
#include "hw2_scenes.h"

using namespace hw2;

Image3 hw_2_1(const std::vector<std::string> &params) {
    // Homework 2.1: render a single 3D triangle

    Image3 img(640 /* width */, 480 /* height */);
    Image3 img_4x(4*img.width, 4*img.height);

    Vector3 p0_3D{0, 0, -1};
    Vector3 p1_3D{1, 0, -1};
    Vector3 p2_3D{0, 1, -1};
    Real s = 1; // scaling factor of the view frustrum
    Real a = Real(img.width)/Real(img.height); // aspect ratio
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    Vector3 background = Vector3{0.5, 0.5, 0.5};
    Real z_near = 1e-6; // distance of the near clipping plane
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-p0") {
            p0_3D.x = std::stof(params[++i]);
            p0_3D.y = std::stof(params[++i]);
            p0_3D.z = std::stof(params[++i]);
        } else if (params[i] == "-p1") {
            p1_3D.x = std::stof(params[++i]);
            p1_3D.y = std::stof(params[++i]);
            p1_3D.z = std::stof(params[++i]);
        } else if (params[i] == "-p2") {
            p2_3D.x = std::stof(params[++i]);
            p2_3D.y = std::stof(params[++i]);
            p2_3D.z = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        }
    }
    
    // project vertices to image plane (camera space)
    auto pToCam = [](const Vector3 &v) { 
        return Vector2{-v.x/v.z, -v.y/v.z}; 
    };
    Vector2 p0_2D = pToCam(p0_3D), p1_2D = pToCam(p1_3D), p2_2D = pToCam(p2_3D);
    // set view frustum & near clipping plane
    auto clip = [&](const Vector2 &v) { 
        return v.x < -s*a && v.x > s*a && v.y < -s && v.y > s; 
    };
    bool p0_clipped = clip(p0_2D), p1_clipped = clip(p1_2D), p2_clipped = clip(p2_2D);
    bool near_clip = -p0_3D.z<z_near && -p1_3D.z<z_near && -p2_3D.z<z_near;
    // convert from camera space to 4x image space
    auto camToImg = [&](const Vector2 &v) { 
        return Vector2{img_4x.width*(v.x+s*a)/(2*s*a), -img_4x.height*(v.y-s)/(2*s)}; 
    };
    Vector2 p0 = camToImg(p0_2D), p1 = camToImg(p1_2D), p2 = camToImg(p2_2D);
    // rasterize 4x image
    for (int y = 0; y < img_4x.height; y++) {
        for (int x = 0; x < img_4x.width; x++) {
            img_4x(x, y) = background;
        }
    }
    if (!(p0_clipped || p1_clipped || p2_clipped || near_clip)) {
        // bounding box
        Real minX = std::min({p0.x, p1.x, p2.x});
        Real maxX = std::max({p0.x, p1.x, p2.x});
        Real minY = std::min({p0.y, p1.y, p2.y});
        Real maxY = std::max({p0.y, p1.y, p2.y});
        Vector2 pMin = Vector2{std::max(minX, Real(0)), std::max(minY, Real(0))};
        Vector2 pMax = Vector2{std::min(maxX, Real(img_4x.width)), 
                               std::min(maxY, Real(img_4x.height))};       
        // edges and normals
        Vector2 e01 = p1 - p0, e12 = p2 - p1, e20 = p0 - p2;
        auto normal = [](const Vector2 &v) { 
            return Vector2{v.y, -v.x}; 
        };
        Vector2 n01 = normal(e01), n12 = normal(e12), n20 = normal(e20);
        // rasterize
        for (int y = pMin.y; y < pMax.y; y++) {
            for (int x = pMin.x; x < pMax.x; x++) {
                Vector2 pixel_center{x + Real(0.5), y + Real(0.5)};
                bool s01 = dot(pixel_center - p0, n01) > 0;
                bool s12 = dot(pixel_center - p1, n12) > 0;
                bool s20 = dot(pixel_center - p2, n20) > 0;
                if ((s01 && s12 && s20) || (!s01 && !s12 && !s20)) {
                    img_4x(x, y) = color;
                }
            }
        }
    }
    // convert to 1x image
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector3 color = {0, 0, 0};
            for (int i = 0; i < 3; i++) {
                color += img_4x(4*x+i, 4*y+i);
            }
            img(x,y) = color/Real(4);
        }
    }
    return img;
}

Image3 hw_2_2(const std::vector<std::string> &params) {
    // Homework 2.2: render a triangle mesh

    Image3 img(640 /* width */, 480 /* height */);

    Real s = 1; // scaling factor of the view frustrum
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }

    TriangleMesh mesh = meshes[scene_id];
    UNUSED(mesh); // silence warning, feel free to remove this

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}

Image3 hw_2_3(const std::vector<std::string> &params) {
    // Homework 2.3: render a triangle mesh with vertex colors

    Image3 img(640 /* width */, 480 /* height */);

    Real s = 1; // scaling factor of the view frustrum
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }

    TriangleMesh mesh = meshes[scene_id];
    UNUSED(mesh); // silence warning, feel free to remove this

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}

Image3 hw_2_4(const std::vector<std::string> &params) {
    // Homework 2.4: render a scene with transformation
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.camera.resolution.x,
               scene.camera.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}

