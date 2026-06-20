import { Version } from '@src/models/Version';


// **** Functions **** //

/**
 * Get latest version
 */
async function getLatest(): Promise<string | null> {
  return '1.2.3';
}

/**
 * Get all versions.
 */
async function getAll(): Promise<Version[]> {
  return [];
}

// **** Export default **** //

export default {
  getLatest,
  getAll,
} as const;
