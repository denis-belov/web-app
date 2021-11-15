// Passing const scalar arguments by refefence
// works unexpectedly in wasm.



#include <cstring>
#include <vector>

#include "xgk-math/src/mat4/mat4.h"



extern "C" void console_log (std::size_t);
#define LOG(x) console_log((std::size_t) x);

// use template

extern "C" void* getVectorData (std::vector<float>& v)
{
	return v.data();
}

extern "C" std::size_t getVectorSize (const std::vector<float>& v)
{
	return v.size();
}



enum class Topology : std::size_t
{
	TRIANGLES,
	POINTS,
	LINES,
};



struct MaterialOptions
{
	Topology topology {};

	char* vertex_shader_code {};

	char* fragment_shader_code {};
};

struct Material
{
	static std::vector<Material*> instances;

	static Material* New (const Topology);

	static void destroy (void);



	// Material () = default;
	Material (void);
	Material (const MaterialOptions&);
	Material (const MaterialOptions&&);



	Topology topology { Topology::POINTS };

	const char* vertex_shader_code
	{R"(
		attribute vec3 a_position;

		void main (void)
		{
			gl_Position = vec4(a_position, 1.0);
		}
	)"};

	const char* fragment_shader_code
	{R"(
		void main (void)
		{
			gl_FragColor = vec4(1.0);
		}
	)"};
};

Material::Material (void)
{
	Material::instances.push_back(this);
}

Material::Material (const MaterialOptions& options)
{
	topology = options.topology;

	Material::instances.push_back(this);
}

Material::Material (const MaterialOptions&& options)
{
	topology = options.topology;

	Material::instances.push_back(this);
}

Material* Material::New (const Topology topology)
{
	Material* material { new Material { { .topology = topology } } };

	return material;
}

void Material::destroy (void)
{
	for (std::size_t i {}; i < Material::instances.size(); ++i)
	{
		delete Material::instances[i];
	}
}

struct SceneObject
{
	std::size_t scene_vertex_data_offset {};
	std::size_t scene_vertex_data_length {};

	std::vector<float> vertex_data
	{
		-1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
	};

	// bool indexed;
	// std::vector<uint32_t> index_data;
	// std::size_t scene_index_data_offset;
	// std::size_t scene_index_data_length;

	// const float vertex_data [9]
	// {
	// 	-1.0f, -1.0f, 0.0f,

	// 	-1.0f, 1.0f, 0.0f,

	// 	1.0f, 1.0f, 0.0f,
	// };
};

struct Scene
{
	std::vector<float> vertex_data;
	// std::vector<uint32_t> index_data;

	void addObject (SceneObject&);
	void addObject (SceneObject*);

	float* getVertexData (void);
};

void Scene::addObject (SceneObject& object)
{
	object.scene_vertex_data_offset = vertex_data.size();
	object.scene_vertex_data_length = object.vertex_data.size();

	vertex_data.resize(vertex_data.size() + object.vertex_data.size());

	memcpy(vertex_data.data() + vertex_data.size() - object.vertex_data.size(), object.vertex_data.data(), object.vertex_data.size());
}

void Scene::addObject (SceneObject* object)
{
	object->scene_vertex_data_offset = vertex_data.size();
	object->scene_vertex_data_length = object->vertex_data.size();

	vertex_data.resize(vertex_data.size() + object->vertex_data.size());

	memcpy(vertex_data.data() + vertex_data.size() - object->vertex_data.size(), object->vertex_data.data(), object->vertex_data.size());
}

float* Scene::getVertexData (void)
{
	return vertex_data.data();
}



Scene* scene {};
Material* material {};
SceneObject* object {};

void destroy (void)
{
	delete object;
	delete material;
	delete scene;
}

int main (void)
{
	scene = new Scene;
	material = new Material { { .topology = Topology::LINES } };
	object = new SceneObject;

	scene->addObject(object);

	LOG(777);
	LOG(&(object->vertex_data));
	LOG(object->vertex_data.data());

	return 0;
}
