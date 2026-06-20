import Url from 'url';

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

/**
 * Get the URL(s) to download an update
 */
async function getUpdateURLs(req: IReq<{ params: { version: string, release: string } }>, res: IRes) {
  const version = req.params.version;
  const release = req.params.release;
  const sub_urls = await VersionService.getUpdateURLs(version, release);
  const urls = sub_urls.map(u =>
    Url.format({
      protocol: req.protocol,
      host: req.get('host'),
      pathname: u,
    })
  );
  return res.status(HttpStatusCodes.OK).json({ 'urls': urls, });
}

// **** Export default **** //

export default {
  getAll,
  getInfo,
  getLatest,
  getUpdateURLs,
} as const;
