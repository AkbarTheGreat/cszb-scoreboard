import EnvVars from '@src/constants/EnvVars';
import fs from 'fs';
import { Version } from '@src/models/Version';

const releaseData = EnvVars.NodeEnv == 'test' ? '../testdata/releases/' : '/releases/';

// **** Functions **** //

/**
 * Get latest version
 */
async function getLatest(): Promise<Version | null> {
  return getInfo((await getAll()).slice(-1)[0])
}

/**
 * Get all versions.
 */
async function getAll(): Promise<string[]> {
  const versions: string[] = fs.readdirSync(releaseData, { withFileTypes: true })
    .filter(item => item.isDirectory() && !item.name.startsWith('.'))
    .map(item => item.name)
  versions.sort((a, b) => versionSorter(a, b))
  return versions;
}

/**
 * Get details for a specific version
 */
async function getInfo(version: string): Promise<Version | null> {
  const allVersions = getAll();
  if (!(await allVersions).includes(version)) {
    return null;
  }
  return new Version(version, await (getReleases(version)))
}

async function getReleases(version: string): Promise<string[]> {
  const releases = fs.readdirSync(releaseData + '/' + version, { withFileTypes: true })
    .filter(item => item.isDirectory() && !item.name.startsWith('.'))
    .map(item => item.name)
  releases.sort();
  return releases;
}

/* I fully believe there's a more idiomatic way to do this, but this at least works, even if it's verbose. */
function versionSorter(a: string, b: string): number {
  const splitA = Version.versionHierarchy(a)
  const splitB = Version.versionHierarchy(b)
  if (splitA.major < splitB.major) {
    return -1
  }
  if (splitA.major > splitB.major) {
    return 1
  }

  if (splitA.minor < splitB.minor) {
    return -1
  }
  if (splitA.minor > splitB.minor) {
    return 1
  }

  if (splitA.sub < splitB.sub) {
    return -1
  }
  if (splitA.sub > splitB.sub) {
    return 1
  }

  if (splitA.tag < splitB.tag) {
    return -1
  }
  if (splitA.tag > splitB.tag) {
    return 1
  }

  return 0;
}

// **** Export default **** //


export default {
  getAll,
  getInfo,
  getLatest,
} as const;

export const _versionSorter = (EnvVars.NodeEnv === "test") ? versionSorter : null;
