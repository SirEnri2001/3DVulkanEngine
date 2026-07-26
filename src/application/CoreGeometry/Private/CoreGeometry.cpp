#define TINYOBJLOADER_IMPLEMENTATION
#define TINYGLTF3_IMPLEMENTATION
#define TINYGLTF3_ENABLE_FS
#define COREGEOMETRY_IMPLEMENT

#include "CoreGeometry.h"
#include "tiny_gltf_v3.h"

bool CoreGeometryImpl::TinyObjLoadObj(tinyobj::attrib_t *attrib, std::vector<tinyobj::shape_t> *shapes,
             std::vector<tinyobj::material_t> *materials, std::string *err,
             const char *filename, const char *mtl_basedir,
             bool triangulate)
{
	return tinyobj::LoadObj(attrib, shapes, materials, err, filename, mtl_basedir, triangulate);
}


namespace CoreGeometryImpl {

int32_t GLTFFindAttribute(const tg3_primitive* prim, const char* name) {
    for (uint32_t i = 0; i < prim->attributes_count; i++) {
        if (tg3_str_equals_cstr(prim->attributes[i].key, name)) {
            return prim->attributes[i].value;
        }
    }
    return -1;
}

float GLTFReadFloat(const tg3_model* model, int32_t accessorIdx,
                           uint64_t elementIndex, int componentIndex) {
    const tg3_accessor* acc = &model->accessors[accessorIdx];
    const tg3_buffer_view* bv = &model->buffer_views[acc->buffer_view];
    const tg3_buffer* buf = &model->buffers[bv->buffer];
    int32_t stride = tg3_accessor_byte_stride(acc, bv);
    const uint8_t* base = buf->data.data + bv->byte_offset + acc->byte_offset;
    int32_t compSize = tg3_component_size(acc->component_type);
    const uint8_t* p = base + elementIndex * (uint64_t)stride
                       + (uint64_t)componentIndex * compSize;
    switch (acc->component_type) {
        case TG3_COMPONENT_TYPE_FLOAT: {
            float v; memcpy(&v, p, sizeof(float)); return v;
        }
        case TG3_COMPONENT_TYPE_BYTE: {
            int8_t v; memcpy(&v, p, sizeof(int8_t));
            return acc->normalized ? (float)v / 127.0f : (float)v;
        }
        case TG3_COMPONENT_TYPE_UNSIGNED_BYTE: {
            uint8_t v; memcpy(&v, p, sizeof(uint8_t));
            return acc->normalized ? (float)v / 255.0f : (float)v;
        }
        case TG3_COMPONENT_TYPE_SHORT: {
            int16_t v; memcpy(&v, p, sizeof(int16_t));
            return acc->normalized ? (float)v / 32767.0f : (float)v;
        }
        case TG3_COMPONENT_TYPE_UNSIGNED_SHORT: {
            uint16_t v; memcpy(&v, p, sizeof(uint16_t));
            return acc->normalized ? (float)v / 65535.0f : (float)v;
        }
        case TG3_COMPONENT_TYPE_INT: {
            int32_t v; memcpy(&v, p, sizeof(int32_t)); return (float)v;
        }
        case TG3_COMPONENT_TYPE_UNSIGNED_INT: {
            uint32_t v; memcpy(&v, p, sizeof(uint32_t)); return (float)v;
        }
        default:
            return 0.0f;
    }
}

uint32_t GLTFReadIndex(const tg3_model* model, int32_t accessorIdx,
                              uint64_t elementIndex) {
    const tg3_accessor* acc = &model->accessors[accessorIdx];
    const tg3_buffer_view* bv = &model->buffer_views[acc->buffer_view];
    const tg3_buffer* buf = &model->buffers[bv->buffer];
    int32_t stride = tg3_accessor_byte_stride(acc, bv);
    const uint8_t* p = buf->data.data + bv->byte_offset + acc->byte_offset
                       + elementIndex * (uint64_t)stride;
    switch (acc->component_type) {
        case TG3_COMPONENT_TYPE_UNSIGNED_BYTE:
            return (uint32_t)(*p);
        case TG3_COMPONENT_TYPE_UNSIGNED_SHORT: {
            uint16_t v; memcpy(&v, p, sizeof(uint16_t)); return (uint32_t)v;
        }
        case TG3_COMPONENT_TYPE_UNSIGNED_INT: {
            uint32_t v; memcpy(&v, p, sizeof(uint32_t)); return v;
        }
        default:
            return 0;
    }
}

bool TinyGltfLoad(tinygltf3::Model& OutModel, const std::string GLTFFileName)
{
    tinygltf3::ErrorStack errors;

    tg3_error_code ret = tinygltf3::parse_file(OutModel, errors, GLTFFileName.c_str());
    if (ret != TG3_OK || errors.has_error()) {
        std::string errMsg = "Failed to load glTF: " + GLTFFileName;
        if (errors.count() > 0) {
            const tg3_error_entry* e = errors.entry(0);
            if (e && e->message) errMsg += std::string(" (") + e->message + ")";
        }
        throw std::runtime_error(errMsg);
    }
    return true;
}

tinygltf3::Model* CreateGltfModel()
{
    return new tinygltf3::Model;
}

void DeleteGltfModel(tinygltf3::Model* model)
{
    delete model;
}

} // namespace CoreGeometryImpl
