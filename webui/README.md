# Herringbone Gear Web UI

原生 HTML/CSS/JavaScript 前端，使用 WebGL 预览，并通过 Emscripten
WebAssembly 在浏览器本地运行项目原有的 C++ 齿形与 CAM 算法。

## 前置条件

- CMake 3.20+
- Ninja
- Emscripten SDK（`emcc`、`emcmake` 可在当前终端使用）
- Python 3（仅用于启动本地静态服务器）

## 构建

在项目根目录执行：

```powershell
emcmake cmake -S . -B out/build/web -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build out/build/web --target gear_web
```

产物会写入 `webui/dist/`：

- `gear-wasm.js`
- `gear-wasm.wasm`

## 本地运行

浏览器安全策略不允许直接用 `file://` 装载 WASM，需要静态服务器：

```powershell
python -m http.server 8080 -d webui
```

然后访问 <http://localhost:8080>。

## 架构

- `wasm/wasm_api.cpp`：稳定的 C ABI，输出网格数组和 NC 文本。
- `src/app.js`：参数读取、WASM 调用、STL/NC 下载。
- `src/renderer.js`：无第三方依赖的 WebGL 网格渲染和轨道控制。
- `styles.css`：响应式工业工作台界面。

生成和导出均在浏览器本地完成，不上传参数或模型。部署时只需把
`webui/` 作为静态目录发布，并确保服务器以 `application/wasm` MIME 类型返回
`.wasm` 文件。

## GitHub Pages

推送到 `main` 后，`.github/workflows/deploy-pages.yml` 会在 GitHub Actions
中使用 Emscripten 6.0.3 重新构建 WASM，并将本目录部署到 GitHub Pages。
工作流也支持在 Actions 页面手动运行。

首次使用时，请在仓库 **Settings → Pages → Build and deployment → Source**
中选择 **GitHub Actions**。站点地址为：

<https://rogerchen2005.github.io/HerringboneGear/>
