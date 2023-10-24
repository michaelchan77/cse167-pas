#include "hw2.h"
#include "hw2_scenes.h"

using namespace hw2;

// void rasterize_triangle (Image3 &img, 
//                          Vector2 &p0, 
//                          Vector2 &p1, 
//                          Vector2 &p2, 
//                          Vector3 &color) 
// {
//     // bounding box
//     Real minX = std::min({p0.x, p1.x, p2.x});
//     Real maxX = std::max({p0.x, p1.x, p2.x});
//     Real minY = std::min({p0.y, p1.y, p2.y});
//     Real maxY = std::max({p0.y, p1.y, p2.y});
//     Vector2 pMin = Vector2{std::max(minX, Real(0)), std::max(minY, Real(0))};
//     Vector2 pMax = Vector2{std::min(maxX, Real(img.width)), 
//                             std::min(maxY, Real(img.height))};
//     // edges and normals
//     Vector2 e01 = p1-p0, n01 = Vector2{e01.y, -e01.x};
//     Vector2 e12 = p2-p1, n12 = Vector2{e12.y, -e12.x};
//     Vector2 e20 = p0-p2, n20 = Vector2{e20.y, -e20.x};
//     // rasterize
//     for (int y = pMin.y; y < pMax.y; y++) {
//         for (int x = pMin.x; x < pMax.x; x++) {
//             Vector2 q = Vector2{x, y};
//             Vector2 q01 = q-p0, q12 = q-p1, q20 = q-p2;
//             bool isPos = dot(q01,n01)>0 && dot(q12,n12)>0 && dot(q20,n20)>0;
//             bool isNeg = dot(q01,n01)<0 && dot(q12,n12)<0 && dot(q20,n20)<0;
//             if (isPos || isNeg) {
//                 img(x, y) = color;
//             }
//         }
//     }
// } 

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
    Vector3 background = Vector3{0.5, 0.5, 0.5};
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
    
    // project vertices to image plane (camera space)
    Vector2 p0_2D = Vector2{-Real(p0.x)/Real(p0.z), -Real(p0.y)/Real(p0.z)};
    Vector2 p1_2D = Vector2{-Real(p1.x)/Real(p1.z), -Real(p1.y)/Real(p1.z)};
    Vector2 p2_2D = Vector2{-Real(p2.x)/Real(p2.z), -Real(p2.y)/Real(p2.z)};
    // set view frustum & near clipping plane
    bool outOfView = p0_2D.x<-s*a && p0_2D.x>s*a && p0_2D.y<-s && p0_2D.x>s
                  && p1_2D.x<-s*a && p1_2D.x>s*a && p1_2D.y<-s && p1_2D.x>s
                  && p2_2D.x<-s*a && p2_2D.x>s*a && p2_2D.y<-s && p2_2D.x>s
                  && -p0.z<z_near && -p1.z<z_near && -p2.z<z_near;
    // convert from camera space to 4x image space
    p0_2D.x = img_4x.width*(p0_2D.x+s*a)/(2*s*a);
    p0_2D.y = -img_4x.height*(p0_2D.y-s)/(2*s);
    p1_2D.x = img_4x.width*(p1_2D.x+s*a)/(2*s*a);
    p1_2D.y = -img_4x.height*(p1_2D.y-s)/(2*s);
    p2_2D.x = img_4x.width*(p2_2D.x+s*a)/(2*s*a);
    p2_2D.y = -img_4x.height*(p2_2D.y-s)/(2*s);
    // rasterize 4x image
    for (int y = 0; y < img_4x.height; y++) {
        for (int x = 0; x < img_4x.width; x++) {
            img_4x(x, y) = background;
        }
    }
    if (!outOfView) {
        // bounding box
        Real minX = std::min({p0_2D.x, p1_2D.x, p2_2D.x});
        Real maxX = std::max({p0_2D.x, p1_2D.x, p2_2D.x});
        Real minY = std::min({p0_2D.y, p1_2D.y, p2_2D.y});
        Real maxY = std::max({p0_2D.y, p1_2D.y, p2_2D.y});
        Vector2 pMin = Vector2{std::max(minX, Real(0)), std::max(minY, Real(0))};
        Vector2 pMax = Vector2{std::min(maxX, Real(img_4x.width)), 
                               std::min(maxY, Real(img_4x.height))};       
        // edges and normals
        Vector2 e01 = p1_2D-p0_2D, e12 = p2_2D-p1_2D, e20 = p0_2D-p2_2D;
        auto normal = [](const Vector2 &v) { return Vector2{v.y, -v.x}; };
        Vector2 n01 = normal(e01), n12 = normal(e12), n20 = normal(e20);
        // rasterize
        for (int y = pMin.y; y < pMax.y; y++) {
            for (int x = pMin.x; x < pMax.x; x++) {
                Vector2 pixel_center{x + Real(0.5), y + Real(0.5)};
                bool s01 = dot(pixel_center - p0_2D, n01) > 0;
                bool s12 = dot(pixel_center - p1_2D, n12) > 0;
                bool s20 = dot(pixel_center - p2_2D, n20) > 0;
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

