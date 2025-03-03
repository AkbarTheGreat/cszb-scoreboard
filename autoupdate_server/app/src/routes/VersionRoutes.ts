import HttpStatusCodes from '@src/constants/HttpStatusCodes';

import VersionService from '@src/services/VersionService';
import { Version } from '@src/models/Version';
import { IReq, IRes } from './types/express/misc';


// **** Functions **** //

/**
 * Get latest Version
 */
async function getLatest(_: IReq, res: IRes) {
  const latest = await VersionService.getLatest();
  return res.status(HttpStatusCodes.OK).json({ 'latest': latest });
}

/**
 * Get information about a version
 */
async function getInfo(req: IReq<{ params: { version: string } }>, res: IRes) {
  const version = req.params.version;
  const info = await VersionService.getInfo(version);
  return res.status(HttpStatusCodes.OK).json({ 'version': info });
}

/**
 * Get all Versions
 */
async function getAll(_: IReq, res: IRes) {
  const versions = await VersionService.getAll();
  return res.status(HttpStatusCodes.OK).json({ 'versions': versions });
}



// **** Export default **** //

export default {
  getAll,
  getInfo,
  getLatest,
} as const;
