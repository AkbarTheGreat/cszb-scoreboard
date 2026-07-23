import Url from 'url';

import HttpStatusCodes from '@src/constants/HttpStatusCodes';
import VersionService from '@src/services/VersionService';
import { GetLatestResponse, GetInfoResponse, GetAllResponse, GetUpdateURLsResponse } from '@proto/version_info';
import { IReq, IRes } from './types/express/misc';


// **** Functions **** 

async function sendBinaryProto(res: IRes, data: Uint8Array, messageType: string, httpStatus: HttpStatusCodes) {
  res.writeHead(httpStatus, {
    'Content-Type': `application/x-protobuf; messageType="${messageType}".`,
    'Content-Length': data.length
  });
  res.end(Buffer.from(data), 'binary');
  return res;
};

/**
 * Get latest Version
 */
async function getLatest(_: IReq, res: IRes) {
  const latest = await VersionService.getLatest();
  return await sendBinaryProto(
    res,
    GetLatestResponse.toBinary(latest),
    GetLatestResponse.typeName,
    latest.error ? HttpStatusCodes.NOT_FOUND : HttpStatusCodes.OK
  );
}

/**
 * Get information about a version
 */
async function getInfo(req: IReq<{ params: { version: string } }>, res: IRes) {
  const version = req.params.version;
  const info = await VersionService.getInfo(version);
  return await sendBinaryProto(
    res,
    GetInfoResponse.toBinary(info),
    GetInfoResponse.typeName,
    info.error ? HttpStatusCodes.NOT_FOUND : HttpStatusCodes.OK
  );
}

/**
 * Get all Versions
 */
async function getAll(_: IReq, res: IRes) {
  const versions = await VersionService.getAll();
  return await sendBinaryProto(
    res,
    GetAllResponse.toBinary(versions),
    GetAllResponse.typeName,
    versions.error ? HttpStatusCodes.NOT_FOUND : HttpStatusCodes.OK
  );
}

/**
 * Get the URL(s) to download an update
 */
async function getUpdateURLs(req: IReq<{ params: { version: string, release: string } }>, res: IRes) {
  const version = req.params.version;
  const release = req.params.release;
  const initialResponse = await VersionService.getUpdateURLs(version, release);
  const response: GetUpdateURLsResponse = {
    urls: initialResponse.urls.map(u =>
      Url.format({
        protocol: req.protocol,
        host: req.get('host'),
        pathname: u,
      })
    ),
    error: initialResponse.error
  };
  return await sendBinaryProto(
    res,
    GetUpdateURLsResponse.toBinary(response),
    GetUpdateURLsResponse.typeName,
    response.error ? HttpStatusCodes.NOT_FOUND : HttpStatusCodes.OK
  );
}

// **** Export default **** //

export default {
  getAll,
  getInfo,
  getLatest,
  getUpdateURLs,
} as const;
