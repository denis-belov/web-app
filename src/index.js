/*
eslint-disable

max-statements,
*/



import './index.scss';
import '@babel/polyfill';

import TestWorker from 'worker-loader!./test.worker.js';

import WasmWrapper from '../../xgk-js/src/wasm-wrapper.js';
import WebGLRenderer from '../../xgk-js/src/webgl-renderer.js';
import WebGPURenderer from '../../xgk-js/src/webgpu-renderer.js';

import wasm_code from './cpp/src/entry-wasm32.cpp';



const threads = [ ...new Array(2) ].map(() => new TestWorker());



window.addEventListener
(
	'load',

	async () =>
	{
		LOG(window.navigator.hardwareConcurrency);

		const memory = new WebAssembly.Memory({ initial: 2, maximum: 2, shared: true });

		const wasm = new WasmWrapper();

		await wasm.init(wasm_code, memory);

		wasm.exports.setWindowSize(window.innerWidth / 2, window.innerHeight);

		wasm.exports.initTransitionStack();

		wasm.exports._Z23constructXgkApiWrappersv();



		threads.forEach
		((thread, thread_index) => thread.postMessage({ id: thread_index, code: wasm_code, memory }));



		const scene_addr = wasm.Addr(wasm.exports.scene.value);
		const material_addr = wasm.Addr(wasm.exports.material.value);
		const material2_addr = wasm.Addr(wasm.exports.material2.value);
		const uniform_block0_addr = wasm.Addr(wasm.exports.uniform_block0.value);
		const object_addr = wasm.Addr(wasm.exports.object.value);
		const object2_addr = wasm.Addr(wasm.exports.object2.value);
		const desc_set1_addr = wasm.Addr(wasm.exports.desc_set1.value);
		const desc_set2_addr = wasm.Addr(wasm.exports.desc_set2.value);



		const orbit = wasm.Addr(wasm.exports.orbit.value);
		const orbit2 = wasm.Addr(wasm.exports.orbit2.value);



		// wasm.exports.initTransitionStack();

		const updateOrbit = () =>
		{
			// wasm.exports._ZN3XGK4MATH5Orbit7rotate2Eff(orbit, 0.01, 0.01);
			// wasm.exports._ZN3XGK4MATH5Orbit6updateEv(orbit);

			// wasm.exports._ZN3XGK4MATH5Orbit7rotate2Eff(orbit2, 0.01, 0.01);
			// wasm.exports._ZN3XGK4MATH5Orbit6updateEv(orbit2);

			// wasm.exports.updateTransitions();

			requestAnimationFrame(updateOrbit);
		};

		updateOrbit();

		setTimeout(wasm.exports.startTransition, 3000);
		setTimeout(wasm.exports.startTransition2, 4000);

		// setInterval(wasm.exports.logStacks, 100);



		window.addEventListener
		(
			'mousemove',

			(evt) =>
			{
			// 	wasm.exports._ZN3XGK4MATH5Orbit7rotate2Eff(orbit, evt.movementX * 0.01, evt.movementY * 0.01);
			// 	wasm.exports._ZN3XGK4MATH5Orbit6updateEv(orbit);

				// wasm.exports.startTransition();
			},
		);



		{
			// const renderer =
			// 	new WebGLRenderer(wasm, document.querySelectorAll('canvas')[0], 'webgl', window.innerWidth / 3, window.innerHeight);



			// const
			// 	{
			// 		Scene,
			// 		Material,
			// 		Object,
			// 	} = renderer;



			// const scene = Scene.getInstance(scene_addr);
			// const material = Material.getInstance(material_addr);
			// const material2 = Material.getInstance(material2_addr);
			// const _object = Object.getInstance(object_addr);
			// const object2 = Object.getInstance(object2_addr);



			// const gl = renderer._context;



			// const b = gl.createBuffer();

			// gl.bindBuffer(gl.ARRAY_BUFFER, b);
			// gl.bufferData(gl.ARRAY_BUFFER, scene.vertex_data, gl.STATIC_DRAW);
			// gl.vertexAttribPointer(0, 3, gl.FLOAT, 0, 0, 0);

			// gl.enableVertexAttribArray(0);



			// let time = Date.now();

			// const [ fps ] = document.querySelectorAll('.fps');

			// let fps_counter = 0;

			// const render = () =>
			// {
			// 	gl.clear(gl.COLOR_BUFFER_BIT);

			// 	material.use();

			// 	_object.draw();

			// 	material2.use();

			// 	object2.draw();



			// 	if (Math.floor((Date.now() - time) * 0.001))
			// 	{
			// 		fps.innerHTML = fps_counter;

			// 		fps_counter = 0;

			// 		time = Date.now();
			// 	}

			// 	++fps_counter;



			// 	requestAnimationFrame(render);
			// };

			// render();
		}



		{
			const renderer =
				new WebGLRenderer(wasm, document.querySelectorAll('canvas')[1], 'webgl2', window.innerWidth / 3, window.innerHeight);

			const gl = renderer._context;



			const
				{
					Scene,
					Material,
					UniformBlock,
					Object,
				} = renderer;



			const scene = Scene.getInstance(scene_addr);
			const material = Material.getInstance(material_addr);
			const material2 = Material.getInstance(material2_addr);
			const uniform_block0 = UniformBlock.getInstance(uniform_block0_addr);
			const _object = Object.getInstance(object_addr);
			const object2 = Object.getInstance(object2_addr);



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

				uniform_block0.use();

				material.use();

				_object.draw();

				material2.use();

				object2.draw();




				if (Math.floor((Date.now() - time) * 0.001))
				{
					fps.innerHTML = fps_counter;

					fps_counter = 0;

					time = Date.now();
				}

				++fps_counter;



				requestAnimationFrame(render);
			};

			render();
		}



		{
			const renderer =
				new WebGPURenderer
				({
					wasm_wrapper_instance: wasm,
					size: [ window.innerWidth / 3, window.innerHeight ],
					canvas: document.querySelectorAll('canvas')[2],
				});

			await renderer.init();



			const
				{
					Scene,
					Material,
					// UniformBlock,
					DescriptorSet,
					Object,
				} = renderer;



			const scene = Scene.getInstance(scene_addr);
			const material = Material.getInstance(material_addr);
			const material2 = Material.getInstance(material2_addr);
			// const uniform_block0 = UniformBlock.getInstance(uniform_block0_addr);
			const desc_set1 = DescriptorSet.getInstance(desc_set1_addr);
			const desc_set2 = DescriptorSet.getInstance(desc_set2_addr);
			const _object = Object.getInstance(object_addr);
			const object2 = Object.getInstance(object2_addr);

			// LOG(desc_set1)
			// LOG(desc_set2)
			// LOG(UniformBlock.instances)



			const c =
				renderer.device.createBuffer
				({
					size: scene.vertex_data.byteLength,

					usage:
					(
						window.GPUBufferUsage.COPY_DST |
						window.GPUBufferUsage.VERTEX
					),
				});

			renderer.device.queue.writeBuffer(c, 0, scene.vertex_data, 0, scene.vertex_data.length);



			let time = Date.now();

			const [ ,, fps ] = document.querySelectorAll('.fps');

			let fps_counter = 0;

			const render = () =>
			{
				const command_encoder = renderer.device.createCommandEncoder();

				const context_texture = renderer._context.getCurrentTexture();

				renderer.render_pass_encoder =
					command_encoder.beginRenderPass
					({
						colorAttachments:
						[
							{
								view: context_texture.createView(),
								// GPUTextureView resolveTarget;

								loadValue: [ 1, 1, 1, 1 ],
								storeOp: 'store',
							},
						],
					});

				renderer.render_pass_encoder.setVertexBuffer(0, c, 0, scene.vertex_data.byteLength);

				desc_set1.use(0);

				material.use();

				_object.draw();

				desc_set2.use(0);

				material2.use();

				object2.draw();

				renderer.render_pass_encoder.endPass();

				const command_buffer = command_encoder.finish();

				renderer.device.queue.submit([ command_buffer ]);



				if (Math.floor((Date.now() - time) * 0.001))
				{
					fps.innerHTML = fps_counter;

					fps_counter = 0;

					time = Date.now();
				}

				++fps_counter;



				requestAnimationFrame(render);
			};

			render();
		}
	},
);
