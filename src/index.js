import '@babel/polyfill';

import wasm_code from './cpp/src/entry-wasm32.cpp';



const ZERO_64 = new Uint8Array(64);



window.addEventListener
(
	'load',

	async () =>
	{
		let MEM_UI8 = null;

		const wasm_module = await WebAssembly.compile(wasm_code);

		const wasm_module_instance =
			await WebAssembly.instantiate
			(
				wasm_module,

				{
					env:
					{
						__memory_base: 0,
						__table_base: 0,
						memory: new WebAssembly.Memory({ initial: 1 }),

						sin: Math.sin,
						cos: Math.cos,
						tan: Math.tan,

						memcpy (dst, src, len)
						{
							return (MEM_UI8.copyWithin(dst, src, src + len), dst);
						},

						zero (dst)
						{
							MEM_UI8.set(ZERO_64, dst);
						},

						console_log: (x) => LOG('C/C++:', x),

						_Znwm: () => 0, // new
						_ZdlPv: () => 0, // delete
						_ZSt20__throw_length_errorPKc: () => 0,
						memset: () => 0,
						printf: () => 0,
						putchar: () => 0,
					},
				},
			);

		LOG(wasm_module_instance.exports);

		MEM_UI8 = new Uint8Array(wasm_module_instance.exports.memory.buffer);

		const MEM_F32 = new Float32Array(wasm_module_instance.exports.memory.buffer);

		const mat_test = wasm_module_instance.exports.mat_test.value;

		LOG(mat_test)

		LOG(MEM_F32.slice(mat_test / 4, (mat_test / 4) + 16));

		let t = Date.now();

		for (let i = 0; i < 999999; ++i)
		{
			wasm_module_instance.exports._ZN3XGK4MATH4Mat413makeProjPerspEfffff
			(
				mat_test,
				10,
				window.innerWidth / window.innerHeight,
				1,
				2000,
				1,
			);
		}

		LOG(Date.now() - t)

		LOG(MEM_F32.slice(mat_test / 4, (mat_test / 4) + 16));

		t = Date.now();

		for (let i = 0; i < 999999; ++i)
		{
			wasm_module_instance.exports._ZN3XGK4MATH4Mat414makeProjPersp2Efffff
			(
				mat_test,
				10,
				window.innerWidth / window.innerHeight,
				1,
				2000,
				1,
			);
		}

		LOG(Date.now() - t)

		LOG(MEM_F32.slice(mat_test / 4, (mat_test / 4) + 16));
	},
);
