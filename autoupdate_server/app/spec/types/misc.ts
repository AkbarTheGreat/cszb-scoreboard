import { Response } from 'supertest';
import { Version } from '@src/models/Version'

// Misc
export type TReqBody = Record<string, unknown>;
export type TRes = Omit<Response, 'body'> & { body: TBody };
export type TApiCb = (res: TRes) => void;


// Response Body: Add things to the body here over time to prevent 
// typescript errors.
type TBody = {
  [key: string]: unknown;
  versions?: string[];
  version?: Version;
  latest?: Version;
}
