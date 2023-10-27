#include "hw2.h"
#include "hw2_scenes.h"

using namespace hw2;

Image3 hw_2_1(const std::vector<std::string> &params) {
    // Homework 2.1: render a single 3D triangle

    Image3 img(640 /* width */, 480 /* height */);
    Image3 img_4x(4*img.width, 4*img.height);

    Vector3 p0{0, 0, -1};
    Vector3 p1{1, 0, -1};
    Vector3 p2{0, 1, -1};
    Real s = 1; // scaling factor of the view frustrum
    Real a = Real(img.width)/Real(img.height); // aspect ratio
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    Real z_near = 1e-6; // distance of the near clipping plane
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-p0") {
            p0.x = std::stof(params[++i]);
            p0.y = std::stof(params[++i]);
            p0.z = std::stof(params[++i]);
        } else if (params[i] == "-p1") {
            p1.x = std::stof(params[++i]);
            p1.y = std::stof(params[++i]);
            p1.z = std::stof(params[++i]);
        } else if (params[i] == "-p2") {
            p2.x = std::stof(params[++i]);
            p2.y = std::stof(params[++i]);
            p2.z = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        }
    }
    // set 4x image background
    for (int y = 0; y < img_4x.height; y++) {
        for (int x = 0; x < img_4x.width; x++) {
            img_4x(x, y) = Vector3{0.5, 0.5, 0.5};
        }
    }
    // if inside near clipping plane
    if (!(-p0.z<z_near && -p1.z<z_near && -p2.z<z_near)) {
        // project vertices to image plane (camera space)
        auto pToCam = [](const Vector3 &v) { 
            return Vector2{-v.x/v.z, -v.y/v.z}; 
        };
        Vector2 p0_cam = pToCam(p0), 
                p1_cam = pToCam(p1), 
                p2_cam = pToCam(p2);
        // convert from camera space to 4x image space
        auto camToImg = [&](const Vector2 &v) { 
            return Vector2{img_4x.width*(v.x+s*a)/(2*s*a), 
                        -img_4x.height*(v.y-s)/(2*s)}; 
        };
        Vector2 p0_img = camToImg(p0_cam), 
                p1_img = camToImg(p1_cam), 
                p2_img = camToImg(p2_cam);
        // define bounding box
        Real minX = max(std::min({p0_img.x, p1_img.x, p2_img.x}), Real(0));
        Real minY = max(std::min({p0_img.y, p1_img.y, p2_img.y}), Real(0));
        Real maxX = min(std::max({p0_img.x, p1_img.x, p2_img.x}), Real(img_4x.width));
        Real maxY = min(std::max({p0_img.y, p1_img.y, p2_img.y}), Real(img_4x.height));
        // define edges and normals
        Vector2 e01 = p1_img - p0_img, 
                e12 = p2_img - p1_img, 
                e20 = p0_img - p2_img;
        auto normal = [](const Vector2 &v) { 
            return Vector2{v.y, -v.x}; 
        };
        Vector2 n01 = normal(e01), 
                n12 = normal(e12), 
                n20 = normal(e20);
        // check if points hit triangle
        for (int y = minY; y < maxY; y++) {
            for (int x = minX; x < maxX; x++) {
                Vector2 pixel_center{x + Real(0.5), y + Real(0.5)};
                bool s01 = dot(pixel_center - p0_img, n01) > 0;
                bool s12 = dot(pixel_center - p1_img, n12) > 0;
                bool s20 = dot(pixel_center - p2_img, n20) > 0;
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
            Real n = 4;
            for (int dx = 0; dx < n; dx++) {
                for (int dy = 0; dy < n; dy++) {
                    color += img_4x(n*x+dx, n*y+dy);
                }
            }
            img(x,y) = color/(n*n);
        }
    }
    return img;
}

Image3 hw_2_2(const std::vector<std::string> &params) {
    // Homework 2.2: render a triangle mesh

    Image3 img(640 /* width */, 480 /* height */);
    Image3 img_4x(4*img.width, 4*img.height);

    Real a = Real(img.width)/Real(img.height); // aspect ratio
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

    // set 4x image background
    for (int y = 0; y < img_4x.height; y++) {
        for (int x = 0; x < img_4x.width; x++) {
            img_4x(x, y) = Vector3{0.5, 0.5, 0.5};
        }
    }
    // for each 3D triangle
    for (int i = 0; i < mesh.faces.size(); i++) {
        Vector3 p0 = mesh.vertices[mesh.faces[i].x], 
                p1 = mesh.vertices[mesh.faces[i].y], 
                p2 = mesh.vertices[mesh.faces[i].z];
        // if inside near clipping plane
        if (!(-p0.z<z_near && -p1.z<z_near && -p2.z<z_near)) {
            // project vertices to image plane (camera space)
            auto pToCam = [](const Vector3 &v) { 
                return Vector2{-v.x/v.z, -v.y/v.z}; 
            };
            Vector2 p0_cam = pToCam(p0), 
                    p1_cam = pToCam(p1), 
                    p2_cam = pToCam(p2);
            // convert from camera space to 4x image space
            auto camToImg = [&](const Vector2 &v) { 
                return Vector2{img_4x.width*(v.x+s*a)/(2*s*a), 
                            -img_4x.height*(v.y-s)/(2*s)}; 
            };
            Vector2 p0_img = camToImg(p0_cam), 
                    p1_img = camToImg(p1_cam), 
                    p2_img = camToImg(p2_cam);
            // define bounding box
            Real minX = max(std::min({p0_img.x, p1_img.x, p2_img.x}), Real(0));
            Real minY = max(std::min({p0_img.y, p1_img.y, p2_img.y}), Real(0));
            Real maxX = min(std::max({p0_img.x, p1_img.x, p2_img.x}), Real(img_4x.width));
            Real maxY = min(std::max({p0_img.y, p1_img.y, p2_img.y}), Real(img_4x.height));
            // define edges and normals
            Vector2 e01 = p1_img - p0_img, 
                    e12 = p2_img - p1_img, 
                    e20 = p0_img - p2_img;
            auto normal = [](const Vector2 &v) { 
                return Vector2{v.y, -v.x}; 
            };
            Vector2 n01 = normal(e01), 
                    n12 = normal(e12), 
                    n20 = normal(e20);
            // check if points hit triangle
            for (int y = minY; y < maxY; y++) {
                for (int x = minX; x < maxX; x++) {
                    Vector2 pixel_center{x + Real(0.5), y + Real(0.5)};
                    bool s01 = dot(pixel_center - p0_img, n01) > 0;
                    bool s12 = dot(pixel_center - p1_img, n12) > 0;
                    bool s20 = dot(pixel_center - p2_img, n20) > 0;
                    if ((s01 && s12 && s20) || (!s01 && !s12 && !s20)) {
                        img_4x(x, y) = mesh.face_colors[i];
                    }
                }
            }
        }
        
    }
    // convert to 1x image
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector3 color = {0, 0, 0};
            Real n = 4;
            for (int dx = 0; dx < n; dx++) {
                for (int dy = 0; dy < n; dy++) {
                    color += img_4x(n*x+dx, n*y+dy);
                }
            }
            img(x,y) = color/(n*n);
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

