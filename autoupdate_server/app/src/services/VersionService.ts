import VersionRepo from '@src/repos/VersionRepo';
import { Version } from '@src/models/Version';


// **** Variables **** //

export const USER_NOT_FOUND_ERR = 'User not found';


// **** Functions **** //

/**
 * Get latest version
 */
function getLatest(): Promise<Version | null> {
  return VersionRepo.getLatest();
}

function getInfo(version: string): Promise<Version | null> {
  return VersionRepo.getInfo(version);
}

function getAll(): Promise<string[] | null> {
  return VersionRepo.getAll();
}

// **** Export default **** //

export default {
  getAll,
  getInfo,
  getLatest,
} as const;
