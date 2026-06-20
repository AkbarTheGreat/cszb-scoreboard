import VersionRepo from '@src/repos/VersionRepo';
import { Version } from '@src/models/Version';
import { RouteError } from '@src/other/classes';
import HttpStatusCodes from '@src/constants/HttpStatusCodes';


// **** Variables **** //

export const VERSION_NOT_FOUND_ERR = 'Version not found';


// **** Functions **** //

/**
 * Get latest version
 */
async function getLatest(): Promise<Version | null> {
  return VersionRepo.getLatest();
}

async function getInfo(version: string): Promise<Version | null> {
  const info = await VersionRepo.getInfo(version);
  if (!info) {
    throw new RouteError(
      HttpStatusCodes.NOT_FOUND,
      VERSION_NOT_FOUND_ERR,
    );
  }
  return info
}

async function getAll(): Promise<string[] | null> {
  return VersionRepo.getAll();
}

async function getUpdateURLs(version: string, release: string): Promise<string[]> {
  const urls = await VersionRepo.getUpdateURLs(version, release);
  if (urls == null) {
    throw new RouteError(
      HttpStatusCodes.NOT_FOUND,
      VERSION_NOT_FOUND_ERR,
    );
  }
  return urls;
}

// **** Export default **** //

export default {
  getAll,
  getInfo,
  getLatest,
  getUpdateURLs,
} as const;
