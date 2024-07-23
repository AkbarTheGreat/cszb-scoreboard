import EnvVars from '@src/constants/EnvVars';
import Filesystem from '@src/constants/Filesystem';
import Paths from '@src/constants/Paths';
import fs from 'fs';
import { Version } from '@proto/version_info';
import VersionHelper from '@src/models/VersionHelper';

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
  const versions: string[] = fs.readdirSync(Filesystem.ReleaseData, { withFileTypes: true })
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
  return VersionHelper.buildVersion(version, await (getReleases(version)))
}

async function getReleases(version: string): Promise<string[]> {
  const releases = fs.readdirSync(`${Filesystem.ReleaseData}/${version}`, { withFileTypes: true })
    .filter(item => item.isDirectory() && !item.name.startsWith('.'))
    .map(item => item.name)
  releases.sort();
  return releases;
}

/**
 * Get the URL to download an update
 */
async function getUpdateURLs(version: string, release: string): Promise<string[] | null> {
  const allVersions = getAll();
  if (!(await allVersions).includes(version)) {
    return null;
  }
  const knownReleases = await (getReleases(version));
  if (!(knownReleases.includes(release))) {
    return null;
  }

  const paths: string[] = fs.readdirSync(`${Filesystem.ReleaseData}/${version}/${release}`, { withFileTypes: true })
    .filter(item => !item.isDirectory())
    .map(item => `${Paths.Base}${Paths.Update.Data}/${version}/${release}/${item.name}`);
  return paths;
}

/* I fully believe there's a more idiomatic way to do this, but this at least works, even if it's verbose. */
function versionSorter(a: string, b: string): number {
  const splitA = VersionHelper.buildVersionHierarchy(a)
  const splitB = VersionHelper.buildVersionHierarchy(b)
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
  getUpdateURLs,
} as const;

export const _versionSorter = (EnvVars.NodeEnv === "test") ? versionSorter : null;
