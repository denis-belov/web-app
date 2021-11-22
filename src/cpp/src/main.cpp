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

	std::string name {};

	size_t block_index {};
};

struct UniformOffsets
{
	size_t object_addr = offsetof(Uniform, object_addr);
	size_t name = offsetof(Uniform, name);
	size_t block_index = offsetof(Uniform, block_index);
};

UniformOffsets uniform_offsets;



struct UniformBlockOptions
{
	size_t binding {};

	std::string name {};
};



struct UniformBlock
{
	static std::vector<UniformBlock*> instances;

	static void destroy (void);



	// UniformBlock () = default;
	UniformBlock (void);
	UniformBlock (const UniformBlockOptions&);
	UniformBlock (const UniformBlockOptions&&);
	UniformBlock (const UniformBlockOptions*);



	size_t binding {};

	std::string name {};

	std::vector<Uniform*> uniforms {};



	void injectUniform (Uniform&);
	void injectUniform (Uniform&&);
	void injectUniform (Uniform*);
};

struct UniformBlockOffsets
{
	size_t binding = offsetof(UniformBlock, binding);
	size_t name = offsetof(UniformBlock, name);
	size_t uniforms = offsetof(UniformBlock, uniforms);
};

UniformBlockOffsets uniform_block_offsets;

UniformBlock::UniformBlock (void)
{
	UniformBlock::instances.push_back(this);
}

UniformBlock::UniformBlock (const UniformBlockOptions& options)
{
	binding = options.binding;
	name = options.name;

	UniformBlock::instances.push_back(this);
}

UniformBlock::UniformBlock (const UniformBlockOptions&& options)
{
	binding = options.binding;
	name = options.name;

	UniformBlock::instances.push_back(this);
}

void UniformBlock::destroy (void)
{
	for (std::size_t i {}; i < UniformBlock::instances.size(); ++i)
	{
		delete UniformBlock::instances[i];
	}
}

void UniformBlock::injectUniform (Uniform& uniform)
{
	uniforms.push_back(&uniform);
}

void UniformBlock::injectUniform (Uniform&& uniform)
{
	uniforms.push_back(&uniform);
}

void UniformBlock::injectUniform (Uniform* uniform)
{
	uniforms.push_back(uniform);
}



struct MaterialOptions
{
	Topology topology { Topology::TRIANGLES };

	std::string glsl100es_code_vertex
	{R"(
		precision highp int;
		precision highp float;

		attribute vec3 in_position;

		uniform mat4 projection_matrix;
		uniform mat4 view_matrix;

		void main (void)
		{
			gl_Position = vec4(in_position, 1.0);
		}
	)"};

	std::string glsl100es_code_fragment
	{R"(
		precision highp int;
		precision highp float;

		void main (void)
		{
			gl_FragColor = vec4(0.25, 0, 0, 1.0);
		}
	)"};

	std::string glsl300es_code_vertex
	{R"(
		#version 300 es

		precision highp int;
		precision highp float;

		layout (location = 0) in vec3 in_position;

		layout (std140) uniform Camera
		{
			mat4 projection_matrix;
			mat4 view_matrix;
		} camera;

		void main (void)
		{
			gl_Position = camera.projection_matrix * camera.view_matrix * vec4(in_position, 1.0f);
		}
	)"};

	std::string glsl300es_code_fragment
	{R"(
		#version 300 es

		precision highp int;
		precision highp float;

		layout (location = 0) out vec4 fragment_color;

		void main (void)
		{
			fragment_color = vec4(0.25f, 0, 0, 1.0f);
		}
	)"};

	std::string wgsl_code_vertex
	{R"(
		[[block]]
		struct VertexIn
		{
			[[location(0)]] pos : vec3<f32>;
		};

		struct VertexOut
		{
			[[builtin(position)]] pos : vec4<f32>;
		};

		[[stage(vertex)]]
		fn main(input : VertexIn) -> VertexOut
		{
			var output : VertexOut;

			output.pos = vec4<f32>(input.pos, 1.0);

			return output;
		}
	)"};

	std::string wgsl_code_fragment
	{R"(
		[[stage(fragment)]]
		fn main() -> [[location(0)]] vec4<f32>
		{
			return vec4<f32>(1.0, 1.0, 0.0, 1.0);
		}
	)"};
};



struct Material
{
	static std::vector<Material*> instances;

	static void destroy (void);



	// Material () = default;
	Material (void);
	Material (const MaterialOptions&);
	Material (const MaterialOptions&&);
	Material (const MaterialOptions*);



	Topology topology {};

	std::string glsl100es_code_vertex {};
	std::string glsl100es_code_fragment {};
	std::string glsl300es_code_vertex {};
	std::string glsl300es_code_fragment {};
	std::string wgsl_code_vertex {};
	std::string wgsl_code_fragment {};

	std::vector<Uniform*> uniforms {};

	std::vector<UniformBlock*> uniform_blocks {};



	// const Uniform& ?
	void injectUniform (Uniform&);
	void injectUniform (Uniform&&);
	void injectUniform (Uniform*);

	void injectUniformBlock (UniformBlock&);
	void injectUniformBlock (UniformBlock&&);
	void injectUniformBlock (UniformBlock*);
};

struct MaterialOffsets
{
	size_t topology = offsetof(Material, topology);
	size_t glsl100es_code_vertex = offsetof(Material, glsl100es_code_vertex);
	size_t glsl100es_code_fragment = offsetof(Material, glsl100es_code_fragment);
	size_t glsl300es_code_vertex = offsetof(Material, glsl300es_code_vertex);
	size_t glsl300es_code_fragment = offsetof(Material, glsl300es_code_fragment);
	size_t wgsl_code_vertex = offsetof(Material, wgsl_code_vertex);
	size_t wgsl_code_fragment = offsetof(Material, wgsl_code_fragment);
	size_t uniforms = offsetof(Material, uniforms);
	size_t uniform_blocks = offsetof(Material, uniform_blocks);
};

MaterialOffsets material_offsets;

Material::Material (void)
{
	const MaterialOptions options {};

	topology = options.topology;

	glsl100es_code_vertex = options.glsl100es_code_vertex;
	glsl100es_code_fragment = options.glsl100es_code_fragment;
	glsl300es_code_vertex = options.glsl300es_code_vertex;
	glsl300es_code_fragment = options.glsl300es_code_fragment;
	wgsl_code_vertex = options.wgsl_code_vertex;
	wgsl_code_fragment = options.wgsl_code_fragment;

	Material::instances.push_back(this);
}

Material::Material (const MaterialOptions& options)
{
	topology = options.topology;

	glsl100es_code_vertex = options.glsl100es_code_vertex;
	glsl100es_code_fragment = options.glsl100es_code_fragment;
	glsl300es_code_vertex = options.glsl300es_code_vertex;
	glsl300es_code_fragment = options.glsl300es_code_fragment;
	wgsl_code_vertex = options.wgsl_code_vertex;
	wgsl_code_fragment = options.wgsl_code_fragment;

	Material::instances.push_back(this);
}

Material::Material (const MaterialOptions&& options)
{
	topology = options.topology;

	glsl100es_code_vertex = options.glsl100es_code_vertex;
	glsl100es_code_fragment = options.glsl100es_code_fragment;
	glsl300es_code_vertex = options.glsl300es_code_vertex;
	glsl300es_code_fragment = options.glsl300es_code_fragment;
	wgsl_code_vertex = options.wgsl_code_vertex;
	wgsl_code_fragment = options.wgsl_code_fragment;

	Material::instances.push_back(this);
}

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
	uniforms.push_back(&uniform);
}

void Material::injectUniform (Uniform* uniform)
{
	uniforms.push_back(uniform);
}

void Material::injectUniformBlock (UniformBlock& uniform_block)
{
	uniform_blocks.push_back(&uniform_block);
}

void Material::injectUniformBlock (UniformBlock&& uniform_block)
{
	uniform_blocks.push_back(&uniform_block);
}

void Material::injectUniformBlock (UniformBlock* uniform_block)
{
	uniform_blocks.push_back(uniform_block);
}



// add instances
struct SceneObject
{
	std::size_t scene_vertex_data_offset {};
	std::size_t scene_vertex_data_length {};

	std::vector<float> vertex_data
	{
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
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



float window_width {};
float window_height {};

Scene* scene {};
Material* material {};
Material* material2 {};
UniformBlock* uniform_block {};
SceneObject* object {};
SceneObject* object2 {};
XGK::MATH::Orbit* orbit;

// Material* materials [1000] {};
// SceneObject* objects [1000] {};

extern "C" void setWindowSize (const float _window_width, const float _window_height)
{
	window_width = _window_width;
	window_height = _window_height;
}

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
				precision highp int;
				precision highp float;

				void main (void)
				{
					gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
				}
			)",

		.glsl300es_code_fragment =
			R"(
				#version 300 es

				precision highp int;
				precision highp float;

				layout (location = 0) out vec4 fragment_color;

				void main (void)
				{
					fragment_color = vec4(0.0, 1.0, 0.0, 1.0);
				}
			)",

		.wgsl_code_fragment =
			R"(
				[[stage(fragment)]]
				fn main() -> [[location(0)]] vec4<f32>
				{
					return vec4<f32>(1.0, 1.0, 1.0, 1.0);
				}
			)",
	}};

	uniform_block = new UniformBlock{{ .binding = 0, .name= "Camera"  }};

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

	orbit->projection_matrix.makeProjPersp(45.0f, window_width / window_height, 1.0f, 2000.0f, 1.0f);



	material->injectUniform(new Uniform { .object_addr = &(orbit->projection_matrix), .name = "projection_matrix" });
	material->injectUniform(new Uniform { .object_addr = &(orbit->view_matrix), .name = "view_matrix" });

	material2->injectUniform(new Uniform { .object_addr = &(orbit->projection_matrix), .name = "projection_matrix" });
	material2->injectUniform(new Uniform { .object_addr = &(orbit->view_matrix), .name = "view_matrix" });

	uniform_block->injectUniform(new Uniform { .object_addr = &(orbit->projection_matrix), .block_index = 0 });
	uniform_block->injectUniform(new Uniform { .object_addr = &(orbit->view_matrix), .block_index = 16});

	material->injectUniformBlock(uniform_block);
	material2->injectUniformBlock(uniform_block);



	// for (size_t i {}; i < 1000; ++i)
	// {
	// 	materials[i] = new Material {{ .topology = Topology::TRIANGLES }};

	// 	materials[i]->injectUniform(new Uniform { .object_addr = &(orbit->projection_matrix), .name = "projection_matrix" });
	// 	materials[i]->injectUniform(new Uniform { .object_addr = &(orbit->view_matrix), .name = "view_matrix" });

	// 	materials[i]->injectUniformBlock(uniform_block);



	// 	objects[i] = new SceneObject;

	// 	scene->addObject(objects[i]);
	// }



	return 0;
}
