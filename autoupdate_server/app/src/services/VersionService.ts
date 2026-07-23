import VersionRepo from '@src/repos/VersionRepo';
import { GetLatestResponse, GetInfoResponse, GetAllResponse, GetUpdateURLsResponse } from '@proto/version_info';
import { RouteError } from '@src/other/classes';
import HttpStatusCodes from '@src/constants/HttpStatusCodes';


// **** Variables **** //

export const VERSION_NOT_FOUND_ERR = 'Version not found';


// **** Functions **** //

/**
 * Get latest version
 */
async function getLatest(): Promise<GetLatestResponse> {
  const latest = await VersionRepo.getLatest();
  if (!latest) {
    return { error: { errorMessage: VERSION_NOT_FOUND_ERR } };
  }
  return { latest: latest };
}

async function getInfo(version: string): Promise<GetInfoResponse> {
  const info = await VersionRepo.getInfo(version);
  if (!info) {
    return { error: { errorMessage: VERSION_NOT_FOUND_ERR } };
  }
  return { version: info }
}

async function getAll(): Promise<GetAllResponse> {
  return { versions: await VersionRepo.getAll() };
}

async function getUpdateURLs(version: string, release: string): Promise<GetUpdateURLsResponse> {
  const urls = await VersionRepo.getUpdateURLs(version, release);
  if (urls == null) {
    return { urls: [], error: { errorMessage: VERSION_NOT_FOUND_ERR } };
  }
  return { urls: urls };
}

// **** Export default **** //

export default {
  getAll,
  getInfo,
  getLatest,
  getUpdateURLs,
} as const;
