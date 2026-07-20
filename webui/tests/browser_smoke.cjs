const { chromium } = require('playwright');

(async () => {
  const browser = await chromium.launch({ channel: 'chrome', headless: true });
  const page = await browser.newPage({ viewport: { width: 1440, height: 900 }, acceptDownloads: true });
  const errors = [];
  const downloads = [];
  page.on('console', message => {
    if (message.type() === 'error') errors.push(`console: ${message.text()}`);
  });
  page.on('pageerror', error => errors.push(`page: ${error.message}`));
  page.on('download', download => downloads.push(download.suggestedFilename()));

  await page.goto('http://127.0.0.1:8765/', { waitUntil: 'networkidle' });
  try {
    await page.waitForFunction(() => document.documentElement.dataset.runtime === 'ready', null, { timeout: 10000 });
  } catch (error) {
    console.error(JSON.stringify({
      runtime: await page.evaluate(() => document.documentElement.dataset.runtime),
      message: await page.locator('#message').innerText(),
      errors,
    }, null, 2));
    throw error;
  }
  await page.click('#generate');
  try {
    await page.waitForFunction(() => document.querySelector('#vertexStat')?.textContent !== '—', null, { timeout: 10000 });
  } catch (error) {
    console.error(JSON.stringify({
      message: await page.locator('#message').innerText(),
      errors,
    }, null, 2));
    throw error;
  }
  await page.click('#saveStl');
  await page.click('[data-tab="cam"]');
  await page.click('#generateCam');
  await page.waitForTimeout(500);

  const result = {
    runtime: await page.evaluate(() => document.documentElement.dataset.runtime),
    vertices: await page.locator('#vertexStat').innerText(),
    faces: await page.locator('#faceStat').innerText(),
    time: await page.locator('#timeStat').innerText(),
    message: await page.locator('#message').innerText(),
    downloads,
    errors,
  };
  await page.screenshot({ path: 'out/webui-browser-smoke.png', fullPage: true });
  await browser.close();

  if (errors.length || !downloads.includes('herringbone_gear.stl') ||
      !downloads.includes('rough.nc') || !downloads.includes('finish.nc')) {
    console.error(JSON.stringify(result, null, 2));
    process.exit(1);
  }
  console.log(JSON.stringify(result, null, 2));
})().catch(error => {
  console.error(error);
  process.exit(1);
});
