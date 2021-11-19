/*
eslint-disable

max-statements,
*/



import './index.scss';
import '@babel/polyfill';

import WasmWrapper from '../../xgk-js/src/wasm-wrapper.js';
import WebGLRenderer from '../../xgk-js/src/webgl-renderer.js';
import WebGPURenderer from '../../xgk-js/src/webgpu-renderer.js';

import wasm_code from './cpp/src/entry-wasm32.cpp';



window.addEventListener
(
	'load',

	async () =>
	{
		const wasm = new WasmWrapper();

		await wasm.init(wasm_code);

		wasm.exports.setWindowSize(window.innerWidth / 2, window.innerHeight);

		wasm.exports.main();



		const scene_addr = wasm.Addr(wasm.exports.scene.value);
		const material_addr = wasm.Addr(wasm.exports.material.value);
		const material2_addr = wasm.Addr(wasm.exports.material2.value);
		const object_addr = wasm.Addr(wasm.exports.object.value);
		const object2_addr = wasm.Addr(wasm.exports.object2.value);



		const orbit = wasm.Addr(wasm.exports.orbit.value);



		const updateOrbit = () =>
		{
			wasm.exports._ZN3XGK4MATH5Orbit7rotate2Eff(orbit, 0.01, 0.01);
			wasm.exports._ZN3XGK4MATH5Orbit6updateEv(orbit);

			requestAnimationFrame(updateOrbit);
		};

		updateOrbit();



		{
			const webgl_renderer = new WebGLRenderer(wasm, document.querySelectorAll('canvas')[0], 'webgl', window.innerWidth / 2, window.innerHeight);

			const gl = webgl_renderer._context;



			const scene = new webgl_renderer.Scene(scene_addr);
			const material = new webgl_renderer.Material(material_addr);
			const material2 = new webgl_renderer.Material(material2_addr);
			const _object = new webgl_renderer.Object(object_addr);
			const object2 = new webgl_renderer.Object(object2_addr);



			// const _materials =
			// 	Array.from(wasm.Addrv(wasm.exports.materials.value, 1000))
			// 		.map
			// 		(
			// 			(addr) =>
			// 			{
			// 				return new webgl_renderer.Material(addr);
			// 			},
			// 		);

			// const _objects =
			// 	Array.from(wasm.Addrv(wasm.exports.objects.value, 1000))
			// 		.map
			// 		(
			// 			(object_addr) =>
			// 			{
			// 				return new webgl_renderer.Object(object_addr);
			// 			},
			// 		);



			const b = gl.createBuffer();

			gl.bindBuffer(gl.ARRAY_BUFFER, b);
			gl.bufferData(gl.ARRAY_BUFFER, scene.vertex_data, gl.STATIC_DRAW);
			gl.vertexAttribPointer(0, 3, gl.FLOAT, 0, 0, 0);

			gl.enableVertexAttribArray(0);



			let time = Date.now();

			const [ fps ] = document.querySelectorAll('.fps');

			let fps_counter = 0;

			const render = () =>
			{
				gl.clear(gl.COLOR_BUFFER_BIT);

				material.use();

				_object.draw();

				material2.use();

				object2.draw();

				// for (let i = 0; i < _materials.length; ++i)
				// {
				// 	gl.clear(gl.COLOR_BUFFER_BIT);
				// 	_materials[i].use();
				// 	_objects[i].draw();
				// }

				requestAnimationFrame(render);



				if (Math.floor((Date.now() - time) * 0.001))
				{
					fps.innerHTML = fps_counter;

					fps_counter = 0;

					time = Date.now();
				}

				++fps_counter;
			};

			render();
		}



		{
			const webgl_renderer = new WebGLRenderer(wasm, document.querySelectorAll('canvas')[1], 'webgl2', window.innerWidth / 2, window.innerHeight);

			const gl = webgl_renderer._context;



			const scene = new webgl_renderer.Scene(scene_addr);
			const material = new webgl_renderer.Material(material_addr);
			const material2 = new webgl_renderer.Material(material2_addr);
			const uniform_block = new webgl_renderer.UniformBlock(wasm.Addr(wasm.exports.uniform_block.value));
			const _object = new webgl_renderer.Object(object_addr);
			const object2 = new webgl_renderer.Object(object2_addr);



			// const _materials =
			// 	Array.from(wasm.Addrv(wasm.exports.materials.value, 100))
			// 		.map
			// 		(
			// 			(addr) =>
			// 			{
			// 				return new webgl_renderer.Material(addr);
			// 			},
			// 		);

			// const _objects =
			// 	Array.from(wasm.Addrv(wasm.exports.objects.value, 100))
			// 	.map
			// 	(
			// 		(object_addr) =>
			// 		{
			// 			return new webgl_renderer.Object(object_addr);
			// 		},
			// 	);



			const b = gl.createBuffer();

			gl.bindBuffer(gl.ARRAY_BUFFER, b);
			gl.bufferData(gl.ARRAY_BUFFER, scene.vertex_data, gl.STATIC_DRAW);
			gl.vertexAttribPointer(0, 3, gl.FLOAT, 0, 0, 0);

			gl.enableVertexAttribArray(0);



			let time = Date.now();

			const [ , fps ] = document.querySelectorAll('.fps');

			let fps_counter = 0;

			const render = () =>
			{
				gl.clear(gl.COLOR_BUFFER_BIT);

				uniform_block.use();

				material.use();

				_object.draw();

				material2.use();

				object2.draw();

				// for (let i = 0; i < _materials.length; ++i)
				// {
				// 	gl.clear(gl.COLOR_BUFFER_BIT);
				// 	_materials[i].use();
				// 	_objects[i].draw();
				// }

				requestAnimationFrame(render);



				if (Math.floor((Date.now() - time) * 0.001))
				{
					fps.innerHTML = fps_counter;

					fps_counter = 0;

					time = Date.now();
				}

				++fps_counter;
			};

			render();
		}



		{
			const webggpu_renderer = new WebGPURenderer(wasm, null,  window.innerWidth / 2, window.innerHeight);

			LOG(webggpu_renderer)

			await webggpu_renderer.init();



			// let time = Date.now();

			// const [ , fps ] = document.querySelectorAll('.fps');

			// let fps_counter = 0;

			// const render = () =>
			// {
			// 	requestAnimationFrame(render);



			// 	if (Math.floor((Date.now() - time) * 0.001))
			// 	{
			// 		fps.innerHTML = fps_counter;

			// 		fps_counter = 0;

			// 		time = Date.now();
			// 	}

			// 	++fps_counter;
			// };

			// render();
		}
	},
);
