// Passing const scalar arguments by refefence
// works unexpectedly in wasm.



#include <cstring>
#include <vector>
#include <string>

#include "xgk-math/src/mat4/mat4.h"
#include "xgk-math/src/orbit/orbit.h"



extern "C" void console_log (std::size_t);
#define LOG(x) console_log((std::size_t) x);

extern "C" std::size_t getTime (void);

// use templates

extern "C" void* getStdVectorDataFloat (std::vector<float>& v)
{
	return v.data();
}

extern "C" std::size_t getStdVectorSizeFloat (std::vector<float>& v)
{
	return v.size();
}

extern "C" void* getStdVectorDataAddr (std::vector<void*>& v)
{
	return v.data();
}

extern "C" std::size_t getStdVectorSizeAddr (std::vector<void*>& v)
{
	return v.size();
}

extern "C" void* getStdStringData (std::string& s)
{
	return s.data();
}

extern "C" std::size_t getStdStringSize (std::string& s)
{
	return s.size();
}



enum class Topology : std::size_t
{
	TRIANGLES,
	POINTS,
	LINES,
};



struct Uniform
{
	void* object_addr {};

	std::string name { "view_matrix" };
};

struct MaterialOptions
{
	Topology topology {};

	std::string glsl100es_code_vertex
	{R"(
		attribute vec3 a_position;

		uniform mat4 projection_matrix;
		uniform mat4 view_matrix;

		void main (void)
		{
			gl_Position = projection_matrix * view_matrix * vec4(a_position, 1.0);
		}
	)"};

	std::string glsl100es_code_fragment
	{R"(
		void main (void)
		{
			gl_FragColor = vec4(0.25, 0, 0, 1.0);
		}
	)"};

	std::string glsl300es_code_vertex
	{R"(
		#version 300es

		layout (location = 0) in vec3 a_position;

		void main (void)
		{
			gl_Position = vec4(a_position, 1.0f);
		}
	)"};

	std::string glsl300es_code_fragment
	{R"(
		#version 300es

		layout (location = 0) out vec4 fragment_color;

		void main (void)
		{
			fragment_color = vec4(0.25f, 0, 0, 1.0f);
		}
	)"};
};

struct Material
{
	static std::vector<Material*> instances;

	// static Material* New (const Topology);

	static void destroy (void);



	// Material () = default;
	Material (void);
	Material (const MaterialOptions&);
	Material (const MaterialOptions&&);
	Material (const MaterialOptions*);



	Topology topology { Topology::POINTS };

	std::string glsl100es_code_vertex
	{R"(
		attribute vec3 a_position;

		uniform mat4 projection_matrix;
		uniform mat4 view_matrix;

		void main (void)
		{
			gl_Position = vec4(a_position, 1.0);
		}
	)"};

	std::string glsl100es_code_fragment
	{R"(
		void main (void)
		{
			gl_FragColor = vec4(0.25, 0, 0, 1.0);
		}
	)"};

	std::string glsl300es_code_vertex
	{R"(
		#version 300es

		layout (location = 0) in vec3 a_position;

		void main (void)
		{
			gl_Position = vec4(a_position, 1.0f);
		}
	)"};

	std::string glsl300es_code_fragment
	{R"(
		#version 300es

		layout (location = 0) out vec4 fragment_color;

		void main (void)
		{
			fragment_color = vec4(0.25f, 0, 0, 1.0f);
		}
	)"};

	std::vector<Uniform*> uniforms {};



	void injectUniform (Uniform&);
	void injectUniform (Uniform&&);
	void injectUniform (Uniform*);
};

Material::Material (void)
{
	Material::instances.push_back(this);
}

Material::Material (const MaterialOptions& options)
{
	topology = options.topology;

	glsl100es_code_vertex = options.glsl100es_code_vertex;
	glsl100es_code_fragment = options.glsl100es_code_fragment;

	Material::instances.push_back(this);
}

Material::Material (const MaterialOptions&& options)
{
	topology = options.topology;

	glsl100es_code_vertex = options.glsl100es_code_vertex;
	glsl100es_code_fragment = options.glsl100es_code_fragment;

	Material::instances.push_back(this);
}

// Material* Material::New (const Topology topology)
// {
// 	Material* material { new Material { { .topology = topology } } };

// 	return material;
// }

void Material::destroy (void)
{
	for (std::size_t i {}; i < Material::instances.size(); ++i)
	{
		delete Material::instances[i];
	}
}

void Material::injectUniform (Uniform& uniform)
{
	uniforms.push_back(&uniform);
}

void Material::injectUniform (Uniform&& uniform)
{
	LOG(uniform.object_addr)
	uniforms.push_back(&uniform);
}

void Material::injectUniform (Uniform* uniform)
{
	uniforms.push_back(uniform);
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
	std::vector<float> vertex_data {};
	// std::vector<uint32_t> index_data;

	void addObject (SceneObject&);
	void addObject (SceneObject*);
};

void Scene::addObject (SceneObject& object)
{
	object.scene_vertex_data_offset = vertex_data.size();
	object.scene_vertex_data_length = object.vertex_data.size();

	std::size_t asd = vertex_data.size();

	vertex_data.resize(vertex_data.size() + object.vertex_data.size());

	memcpy(vertex_data.data() + asd, object.vertex_data.data(), object.vertex_data.size() * 4);
}

void Scene::addObject (SceneObject* object)
{
	object->scene_vertex_data_offset = vertex_data.size();
	object->scene_vertex_data_length = object->vertex_data.size();

	std::size_t asd = vertex_data.size();

	vertex_data.resize(vertex_data.size() + object->vertex_data.size());

	memcpy(vertex_data.data() + asd, object->vertex_data.data(), object->vertex_data.size() * 4);
}



Scene* scene {};
Material* material {};
Material* material2 {};
SceneObject* object {};
SceneObject* object2 {};
XGK::MATH::Orbit* orbit;

extern "C" void destroy (void)
{
	delete object;
	delete material;
	delete scene;
}

int main (void)
{
	scene = new Scene;
	material = new Material {{ .topology = Topology::TRIANGLES }};

	material2 = new Material
	{{
		.topology = Topology::TRIANGLES,

		.glsl100es_code_fragment =
			R"(
				void main (void)
				{
					gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
				}
			)",
	}};

	object = new SceneObject;
	object2 = new SceneObject;

	float data[9]
	{
		-1.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.0f,
		1.0f, 1.0f, 0.0f,
	};

	memcpy(object2->vertex_data.data(), data, 36);

	scene->addObject(*object);
	scene->addObject(*object2);

	orbit = new XGK::MATH::Orbit;

	orbit->object.setTransZ(10.0f);
	orbit->update();

	orbit->projection_matrix.makeProjPersp(45.0f, 800.0f / 600.0f, 1.0f, 2000.0f, 1.0f);



	material->injectUniform(new Uniform { .object_addr = &(orbit->projection_matrix), .name = "projection_matrix" });
	material->injectUniform(new Uniform { .object_addr = &(orbit->view_matrix), .name = "view_matrix" });

	material2->injectUniform(new Uniform { .object_addr = &(orbit->projection_matrix), .name = "projection_matrix" });
	material2->injectUniform(new Uniform { .object_addr = &(orbit->view_matrix), .name = "view_matrix" });



	return 0;
}
