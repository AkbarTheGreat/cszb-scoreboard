/**
 * Remove old files, copy front-end ones.
 */

import fs from 'fs-extra';
import logger from 'jet-logger';
import childProcess from 'child_process';


/**
 * Start
 */
(async () => {
  try {
    // Remove current build
    await remove('./dist/');
    // Copy front-end files
    await copy('./src/public', './dist/public');

    // Build  protobuf files
    await build_protobuf(['./src/proto/version_info.proto']);

    await exec('tsc --build tsconfig.prod.json', './');
  } catch (err) {
    logger.err(err);
    process.exit(1);
  }
})();

/**
 * Build protobuf sources
 */
async function build_protobuf(protos: string[]): Promise<void> {
  await mkdir('./generated');
  await mkdir('./generated/proto');
  // TODO (akbar): Can I call the protoc plugin without using a shell to call npx?  I need to look into this.
  const command_base = 'npx protoc --ts_out ./generated/proto --proto_path src/proto';
  protos.map(async file => await exec(`${command_base} ${file}`, './'));
}

/**
 * Remove file
 */
function remove(loc: string): Promise<void> {
  return new Promise((res, rej) => {
    return fs.remove(loc, (err) => {
      return (!!err ? rej(err) : res());
    });
  });
}

/**
 * Copy file.
 */
function copy(src: string, dest: string): Promise<void> {
  return new Promise((res, rej) => {
    return fs.copy(src, dest, (err) => {
      return (!!err ? rej(err) : res());
    });
  });
}

/**
 * Make directory
 */
function mkdir(dest: string): Promise<void> {
  return new Promise((res, rej) => {
    return fs.mkdir(dest, (err) => {
      // Ignores errors, to avoid failing when the path already exists.
      return res();
    });
  });
}


/**
 * Do command line command.
 */
function exec(cmd: string, loc: string): Promise<void> {
  return new Promise((res, rej) => {
    return childProcess.exec(cmd, { cwd: loc }, (err, stdout, stderr) => {
      if (!!stdout) {
        logger.info(stdout);
      }
      if (!!stderr) {
        logger.warn(stderr);
      }
      return (!!err ? rej(err) : res());
    });
  });
}
