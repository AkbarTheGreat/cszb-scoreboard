import { Response } from 'supertest';
import { Version, GetLatestResponse, GetInfoResponse, GetAllResponse, GetUpdateURLsResponse } from '@proto/version_info'
import consumers from 'stream/consumers'
import { VERSION_NOT_FOUND_ERR } from '@src/services/VersionService';
import { ReadableStreamReadResult } from 'stream/web';

// Misc
export type TReqBody = Record<string, unknown>;

export class FromProto {

  static async getBody(res: Response): Promise<Uint8Array> {
    let body: string = res.text;
    return Uint8Array.from(Array.from(body).map(letter => letter.charCodeAt(0)));
  }

  static async latest(res: Response): Promise<GetLatestResponse> {
    if (res.body) {
      return await this.getBody(res).then(body => {
        return GetLatestResponse.fromBinary(body);
      });
    }
    return { error: { errorMessage: VERSION_NOT_FOUND_ERR } };
  }

  static async info(res: Response): Promise<GetInfoResponse> {
    if (res.body) {
      return await this.getBody(res).then(body => {
        return GetInfoResponse.fromBinary(body);
      });
    }
    return { error: { errorMessage: VERSION_NOT_FOUND_ERR } };
  }

  static async all(res: Response): Promise<GetAllResponse> {
    if (res.body) {
      return await this.getBody(res).then(body => {
        return GetAllResponse.fromBinary(body);
      });
    }
    return { versions: [], error: { errorMessage: VERSION_NOT_FOUND_ERR } };
  }

  static async update(res: Response): Promise<GetUpdateURLsResponse> {
    if (res.body) {
      return await this.getBody(res).then(body => {
        return GetUpdateURLsResponse.fromBinary(body);
      });
    }
    return { urls: [], error: { errorMessage: VERSION_NOT_FOUND_ERR } };
  }
}
export type TApiCb = (res: Response) => void;
