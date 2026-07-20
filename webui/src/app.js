import {GearRenderer} from './renderer.js';

const $=id=>document.getElementById(id),number=id=>Number($(id).value);
const message=$('message');
let Module,mesh,renderer;
try{renderer=new GearRenderer($('canvas'))}catch(error){fail(error.message)}

document.querySelectorAll('.tab').forEach(tab=>tab.addEventListener('click',()=>{document.querySelectorAll('.tab').forEach(x=>x.classList.toggle('active',x===tab));document.querySelectorAll('.panel').forEach(x=>x.classList.toggle('active',x.dataset.panel===tab.dataset.tab))}));

function gearArgs(){return [number('z'),number('m'),number('alpha'),number('beta'),number('face'),number('kt'),number('ka'),number('kr'),number('kz'),number('x'),number('rg')]}
function errorText(){return Module.UTF8ToString(Module._gear_last_error())}
function setMessage(text,type=''){message.textContent=text;message.className=type}
function fail(text){document.documentElement.dataset.runtime='error';setMessage(text,'error')}
function download(data,name,type='application/octet-stream'){const url=URL.createObjectURL(new Blob([data],{type})),a=document.createElement('a');a.href=url;a.download=name;a.click();setTimeout(()=>URL.revokeObjectURL(url),1000)}

async function init(){
  if(typeof createGearModule!=='function')throw new Error('未找到 dist/gear-wasm.js，请先构建 WebAssembly 模块');
  Module=await createGearModule({locateFile:path=>`dist/${path}`});
  document.documentElement.dataset.runtime='ready';setMessage('参数就绪，可以生成模型');$('generate').disabled=false;
}

function generate(){
  if(!Module||!$('parameters').reportValidity())return;
  setMessage('正在计算齿形与三角网格…');const start=performance.now();
  requestAnimationFrame(()=>{try{if(!Module._gear_build_mesh(...gearArgs()))throw new Error(errorText());const vc=Module._gear_vertex_count(),ic=Module._gear_index_count(),vp=Module._gear_vertices(),ip=Module._gear_indices();mesh={vertices:new Float32Array(Module.HEAPF32.buffer,vp,vc*3).slice(),indices:new Uint32Array(Module.HEAPU32.buffer,ip,ic).slice()};renderer.setMesh(mesh.vertices,mesh.indices);const elapsed=performance.now()-start;$('vertexStat').textContent=vc.toLocaleString();$('faceStat').textContent=(ic/3).toLocaleString();$('timeStat').textContent=`${elapsed.toFixed(0)} ms`;$('fps').textContent=`${(mesh.vertices.byteLength+mesh.indices.byteLength)/1048576|0} MB MESH`;$('emptyState').classList.add('hidden');$('saveStl').disabled=false;setMessage('模型生成完成','success')}catch(error){setMessage(error.message,'error')}},20)
}

function makeStl(){
  const {vertices:v,indices:i}=mesh,buffer=new ArrayBuffer(84+i.length/3*50),view=new DataView(buffer);view.setUint32(80,i.length/3,true);let o=84;
  for(let t=0;t<i.length;t+=3){const a=i[t]*3,b=i[t+1]*3,c=i[t+2]*3,ab=[v[b]-v[a],v[b+1]-v[a+1],v[b+2]-v[a+2]],ac=[v[c]-v[a],v[c+1]-v[a+1],v[c+2]-v[a+2]],n=[ab[1]*ac[2]-ab[2]*ac[1],ab[2]*ac[0]-ab[0]*ac[2],ab[0]*ac[1]-ab[1]*ac[0]],l=Math.hypot(...n)||1;n.forEach((x,k)=>view.setFloat32(o+k*4,x/l,true));o+=12;for(const q of[a,b,c])for(let k=0;k<3;k++,o+=4)view.setFloat32(o,v[q+k],true);view.setUint16(o,0,true);o+=2}return buffer
}

function generateCam(){
  if(!Module||!$('parameters').reportValidity())return;
  try{const g=gearArgs(),common=[number('remain'),number('teethCount')];if(!Module._gear_generate_roughing(...g,number('roughDepth'),number('roughCutter'),...common,number('roughTool')))throw new Error(errorText());const rough=Module.UTF8ToString(Module._gear_text(),Module._gear_text_size());if(!Module._gear_generate_finishing(...g,number('finishDepth'),number('finishCutter'),...common,number('cutterHeight'),number('ra'),number('finishTool')))throw new Error(errorText());const finish=Module.UTF8ToString(Module._gear_text(),Module._gear_text_size());download(rough,'rough.nc','text/plain;charset=utf-8');download(finish,'finish.nc','text/plain;charset=utf-8');setMessage('粗加工与精加工 NC 已生成','success')}catch(error){setMessage(error.message,'error')}
}

$('generate').addEventListener('click',generate);$('saveStl').addEventListener('click',()=>mesh&&download(makeStl(),'herringbone_gear.stl'));$('generateCam').addEventListener('click',generateCam);$('fit').addEventListener('click',()=>renderer.fit());$('wireframe').addEventListener('click',e=>e.currentTarget.classList.toggle('active',renderer.toggleWire()));window.addEventListener('keydown',e=>{if(e.key.toLowerCase()==='g'&&!/input/i.test(document.activeElement.tagName))generate()});
init().catch(error=>fail(error.message));
