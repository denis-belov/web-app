// Passing const scalar arguments by refefence
// works unexpectedly in wasm.



#include <cstring>
#include <vector>
#include <string>

#include "xgk-math/src/mat4/mat4.h"
#include "xgk-math/src/orbit/orbit.h"
#include "xgk-math/src/util/util.h"
#include "xgk-api/src/uniform/uniform.h"
#include "xgk-api/src/uniform-block/uniform-block.h"
#include "xgk-aux/src/transition-stack/transition-stack.h"
#include "xgk-aux/src/transition/transition.h"



extern "C" void console_log (std::size_t);
#define LOG(x) console_log((std::size_t) x);

extern "C" void console_log_f (float);
#define LOGF(x) console_log_f((float) x);

extern "C" std::size_t getTime (void);



extern "C" void* getStdVectorData (std::vector<int>& v)
{
	return v.data();
}

extern "C" size_t getStdVectorSize (std::vector<int>& v)
{
	return v.size();
}

extern "C" void* getStdStringData (std::string& s)
{
	return s.data();
}

extern "C" size_t getStdStringSize (std::string& s)
{
	return s.size();
}



// struct DescriptorBinding
// {};

struct DescriptorSet
{
	std::vector<void*> bindings {};
};

struct DescriptorSetOffsets
{
	size_t bindings = offsetof(DescriptorSet, bindings);
};

DescriptorSetOffsets descriptor_set_offsets {};



enum class Topology : size_t
{
	TRIANGLES,
	POINTS,
	LINES,
};



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
			gl_Position = projection_matrix * view_matrix * vec4(in_position, 1.0);
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
		[[block]] struct VertexIn
		{
			[[location(0)]] pos : vec3<f32>;
			[[builtin(vertex_index)]] vi : u32;
		};

		[[block]] struct VertexOut
		{
			[[builtin(position)]] pos : vec4<f32>;
		};

		[[block]] struct Camera
		{
			projection_matrix : mat4x4<f32>;
			view_matrix : mat4x4<f32>;
		};

		[[group(0), binding(0)]] var<uniform> camera : Camera;

		[[block]] struct Dedicated
		{
			w_offset : f32;
		};

		[[group(0), binding(1)]] var<uniform> dedicated : Dedicated;

		[[stage(vertex)]] fn main(input : VertexIn) -> VertexOut
		{
			var output : VertexOut;

			output.pos = camera.projection_matrix * camera.view_matrix * vec4<f32>(input.pos, 1.0 + dedicated.w_offset);

			return output;
		}
	)"};

	std::string wgsl_code_fragment
	{R"(
		[[stage(fragment)]] fn main() -> [[location(0)]] vec4<f32>
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

	std::vector<XGK::API::Uniform*> uniforms {};

	std::vector<XGK::API::UniformBlock*> uniform_blocks {};

	std::vector<DescriptorSet*> descriptor_sets {};

	// XGK::API::UniformBlock dedicated_uniform_block {{ .binding = 0, .name = "Dedicated" }};

	// #include "glsl450_fragment_code.h"



	// const Uniform& ?
	void injectUniform (XGK::API::Uniform&);
	void injectUniform (XGK::API::Uniform&&);
	void injectUniform (XGK::API::Uniform*);

	void injectUniformBlock (XGK::API::UniformBlock&);
	void injectUniformBlock (XGK::API::UniformBlock&&);
	void injectUniformBlock (XGK::API::UniformBlock*);
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
	size_t descriptor_sets = offsetof(Material, descriptor_sets);
	// size_t dedicated_uniform_block = offsetof(Material, dedicated_uniform_block);
	// size_t glsl450_fragment_code = offsetof(Material, glsl450_fragment_code);
};

MaterialOffsets material_offsets;

std::vector<Material*> Material::instances {};

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

// TODO: destroy all dedicated uniforms
void Material::destroy (void)
{
	for (std::size_t i {}; i < Material::instances.size(); ++i)
	{
		delete Material::instances[i];
	}
}

void Material::injectUniform (XGK::API::Uniform& uniform)
{
	uniforms.push_back(&uniform);
}

void Material::injectUniform (XGK::API::Uniform&& uniform)
{
	uniforms.push_back(&uniform);
}

void Material::injectUniform (XGK::API::Uniform* uniform)
{
	uniforms.push_back(uniform);
}

void Material::injectUniformBlock (XGK::API::UniformBlock& uniform_block)
{
	uniform_blocks.push_back(&uniform_block);
}

void Material::injectUniformBlock (XGK::API::UniformBlock&& uniform_block)
{
	uniform_blocks.push_back(&uniform_block);
}

void Material::injectUniformBlock (XGK::API::UniformBlock* uniform_block)
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

extern "C" void setWindowSize (const float _window_width, const float _window_height)
{
	window_width = _window_width;
	window_height = _window_height;
}

Scene* scene {};
Material* material {};
Material* material2 {};
XGK::API::UniformBlock* uniform_block0 {};
XGK::API::UniformBlock* uniform_block1 {};
SceneObject* object {};
SceneObject* object2 {};
XGK::MATH::Orbit* orbit {};
XGK::MATH::Orbit* orbit2 {};
DescriptorSet* desc_set1 {};
DescriptorSet* desc_set2 {};

float curve_values [5000];
XGK::Transition orbit_transition;
XGK::Transition orbit_transition2;

extern "C" void ___test (const size_t& time_gone)
{
	static size_t prev_time {};

	float temp { curve_values[time_gone - 1] };

	if (time_gone < prev_time)
	{
		prev_time = 0;
	}

	orbit->rotation_speed_x = orbit->rotation_speed_y = temp * (time_gone - prev_time) * 0.01;

	// LOGF(temp);

	// LOG(time_gone);
	// LOG(prev_time);

	prev_time = time_gone;

	orbit->rotate();
	orbit->update();
}

extern "C" void ___test2 (const size_t& time_gone)
{
	static size_t prev_time {};

	float temp { curve_values[time_gone - 1] };

	if (time_gone < prev_time)
	{
		prev_time = 0;
	}

	orbit->translation_speed_y = temp * (time_gone - prev_time) * 0.001;

	// LOGF(temp);

	// LOG(time_gone);
	// LOG(prev_time);

	prev_time = time_gone;

	orbit->transY();
	orbit->update();
}

XGK::TransitionStack* _stack0 {};
XGK::TransitionStack* _stack1 {};

extern "C" void initTransitionStack (void)
{
	// curve_values = new float [5000];

	// XGK::MATH::UTIL::makeBezierCurve3Sequence2
	// (
	// 	curve_values,
	// 	0, 0, 0, 0,
	// 	// 0,.5,.5,1,
	// 	// 0, 1, 1, 1,
	// 	1000
	// );

	XGK::MATH::UTIL::makeBezierCurve3Sequence
	(
		curve_values,

		// 0.0000001f, 0.0000001f, 0.0000001f, 0.0f, 0.00000005f, 0.0f, 0.0f, 0.0f,

		1.0f,
		1.0f,
		1.0f,
		0.0f,
		0.5f,
		0.0f,
		0.0f,
		0.0f,

		// 1.0f * 3.14f / 1000000000.0f,
		// 1.0f * 3.14f / 1000000000.0f,
		// 1.0f * 3.14f / 1000000000.0f,
		// 1.0f * 3.14f / 1000000000.0f,
		// 1.0f * 3.14f / 1000000000.0f,
		// 1.0f * 3.14f / 1000000000.0f,
		// 1.0f * 3.14f / 1000000000.0f,
		// 1.0f * 3.14f / 1000000000.0f,

		5000
	);

	_stack0 = new XGK::TransitionStack { 64 };
	_stack1 = new XGK::TransitionStack { 64 };
}

extern "C" void updateTransitions0 (void)
{
	_stack0->calculateFrametime();
	_stack0->update();
}

extern "C" void updateTransitions1 (void)
{
	_stack1->calculateFrametime();
	_stack1->update();
}

extern "C" void logStacks (void)
{
	LOG(_stack0->length)
	LOG(_stack1->length)
}

// extern "C" void updateTransitions2 (void)
// {
// 	while (1)
// 	{
// 		_stack->calculateFrametime();
// 		_stack->update();
// 	}
// }

// extern "C" void startTransition (void)
// {
// 	orbit_transition.start2(5000, ___test);
// }

extern "C" void startTransition (void)
{
	orbit_transition.start2(5000, ___test);
}

extern "C" void startTransition2 (void)
{
	orbit_transition2.start2(5000, ___test2);
}

int main (void)
{
	scene = new Scene;

	material = new Material
	{{
		.topology = Topology::TRIANGLES,

		.wgsl_code_vertex =
			R"(
				[[block]] struct VertexIn
				{
					[[location(0)]] pos : vec3<f32>;
					[[builtin(vertex_index)]] vi : u32;
				};

				[[block]] struct VertexOut
				{
					[[builtin(position)]] pos : vec4<f32>;
				};

				[[block]] struct Camera
				{
					projection_matrix : mat4x4<f32>;
					view_matrix : mat4x4<f32>;
				};

				[[group(0), binding(0)]] var<uniform> camera : Camera;

				[[stage(vertex)]] fn main(input : VertexIn) -> VertexOut
				{
					var output : VertexOut;

					output.pos = camera.projection_matrix * camera.view_matrix * vec4<f32>(input.pos, 1.0);

					return output;
				}
			)",
	}};

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
				[[stage(fragment)]] fn main() -> [[location(0)]] vec4<f32>
				{
					return vec4<f32>(1.0, 0.0, 1.0, 1.0);
				}
			)",
	}};

	uniform_block0 =
		new XGK::API::UniformBlock
		{{
			.binding = 0,
			.type = XGK::API::DescriptorBindingType::UNIFORM_BUFFER,
			.name = "Camera",
		}};

	uniform_block1 =
		new XGK::API::UniformBlock
		{{
			.binding = 1,
			.type = XGK::API::DescriptorBindingType::UNIFORM_BUFFER,
			.name = "Camera2",
		}};

	object = new SceneObject;
	object2 = new SceneObject;

	memcpy(object2->vertex_data.data(), std::vector({ -1.0f, -1.0f, 0.0f, 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f }).data(), 36);

	scene->addObject(*object);
	scene->addObject(*object2);

	orbit = new XGK::MATH::Orbit;

	orbit->object.setTransZ(10.0f);
	orbit->update();

	orbit->projection_matrix.makeProjPersp(45.0f, window_width / window_height, 1.0f, 2000.0f, 1.0f);



	orbit2 = new XGK::MATH::Orbit;

	orbit2->object.setTransZ(10.0f);
	orbit2->update();

	orbit2->projection_matrix.makeProjPersp(45.0f, window_width / window_height, 1.0f, 2000.0f, 1.0f);




	XGK::API::Uniform* projection_matrix_uniform
	{ new XGK::API::Uniform { .object_addr = &(orbit->projection_matrix), .name = "projection_matrix", .block_index = offsetof(XGK::MATH::Orbit, projection_matrix), .size = sizeof(orbit->projection_matrix) } };

	XGK::API::Uniform* view_matrix_uniform
	{ new XGK::API::Uniform { .object_addr = &(orbit->view_matrix), .name = "view_matrix", .block_index = offsetof(XGK::MATH::Orbit, view_matrix), .size = sizeof(orbit->view_matrix) } };

	material->injectUniform(projection_matrix_uniform);
	material->injectUniform(view_matrix_uniform);

	material2->injectUniform(projection_matrix_uniform);
	material2->injectUniform(view_matrix_uniform);

	uniform_block0->injectUniform(projection_matrix_uniform);
	uniform_block0->injectUniform(view_matrix_uniform);

	uniform_block1->injectUniform(new XGK::API::Uniform { .object_addr = &(orbit2->view_matrix), .block_index = 0, .size = sizeof(float) });

	material->injectUniformBlock(uniform_block0);
	material2->injectUniformBlock(uniform_block0);



	desc_set1 = new DescriptorSet;
	desc_set2 = new DescriptorSet;

	desc_set1->bindings.push_back(uniform_block0);

	// TODO: specify binding index at pushing (desc_set2->injectBinding(uniform_block0, 0))
	desc_set2->bindings.push_back(uniform_block0);
	desc_set2->bindings.push_back(uniform_block1);

	material->descriptor_sets.push_back(desc_set1);
	material2->descriptor_sets.push_back(desc_set2);



	return 0;
}
