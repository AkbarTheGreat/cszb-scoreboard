import VersionRepo from '@src/repos/VersionRepo';
import { Version } from '@src/models/Version';


// **** Variables **** //

export const USER_NOT_FOUND_ERR = 'User not found';


// **** Functions **** //

/**
 * Get latest version
 */
function getLatest(): Promise<string | null> {
  return VersionRepo.getLatest();
}

function getAll(): Promise<Version[] | null> {
  return VersionRepo.getAll();
}

// **** Export default **** //

export default {
  getAll,
  getLatest,
} as const;
