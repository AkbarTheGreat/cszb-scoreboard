import supertest, { Test } from 'supertest';
import TestAgent from 'supertest/lib/agent';
import { defaultErrMsg as ValidatorErr } from 'jet-validator';
import insertUrlParams from 'inserturlparams';

import app from '@src/server';

import VersionRepo from '@src/repos/VersionRepo';
import { Version } from '@src/models/Version';
import HttpStatusCodes from '@src/constants/HttpStatusCodes';
import { VERSION_NOT_FOUND_ERR } from '@src/services/VersionService';

import Paths from '@spec/support/Paths';
import apiCb from '@spec/support/apiCb';
import { TApiCb } from 'spec/types/misc';


const getDummyVersions = () => {
  return ['0.5.0', '0.5.0_hotfix', '1.0.0'];
}

const getDummyLatest = () => {
  return new Version('1.0.0', ['win64'])
};

const getDummyUpdatePaths = () => {
  return ['api/update_path/file1', 'api/update_path/file2']
};

// Tests
describe('VersionRouter', () => {

  let agent: TestAgent<Test>;

  // Run before all tests
  beforeAll(done => {
    agent = supertest.agent(app);
    done();
  });

  // Get all versions
  describe(`"GET:${Paths.Versions.All}"`, () => {

    // Setup API
    const api = (cb: TApiCb) =>
      agent
        .get(Paths.Versions.All)
        .end(apiCb(cb));

    // Success
    it('should return a JSON object with all the versions and a status code ' +
      `of "${HttpStatusCodes.OK}" if the request was successful.`, (done) => {
        // Add spy
        const data = getDummyVersions();
        jest.spyOn(VersionRepo, 'getAll').mockResolvedValue(data);
        // Call API
        api(res => {
          expect(res.status).toBe(HttpStatusCodes.OK);
          expect(res.body).toEqual({ versions: data });
          done();
        });
      });
  });

  // Version info
  describe(`"GET:${Paths.Versions.Info}"`, () => {

    // Setup API
    const api = (version: string | null, cb: TApiCb) =>
      agent
        .get(Paths.Versions.Info.replace(':version', version ?? ''))
        .end(apiCb(cb));

    // Success
    it(`should return a status code of "${HttpStatusCodes.OK}" if the ` +
      'request was successful.', (done) => {
        // Add spy
        const versionData = getDummyLatest();
        jest.spyOn(VersionRepo, 'getInfo').mockResolvedValue(versionData);
        // Call api
        api('1.0.0', res => {
          expect(res.status).toBe(HttpStatusCodes.OK);
          expect(res.body).toEqual({ version: versionData });
          done();
        });
      });

    // Version not found
    it('should return a JSON object with the error message of ' +
      `"${VERSION_NOT_FOUND_ERR}" and a status code of ` +
      `"${HttpStatusCodes.NOT_FOUND}" if the id was not found.`, (done) => {
        // Add spy
        jest.spyOn(VersionRepo, 'getInfo').mockResolvedValue(null);
        // Call api
        api('0.0.0', res => {
          expect(res.status).toBe(HttpStatusCodes.NOT_FOUND);
          expect(res.body.error).toBe(VERSION_NOT_FOUND_ERR);
          done();
        });
      });
  });

  // Version update urls
  describe(`"GET:${Paths.Versions.UpdateURL}"`, () => {

    // Setup API
    const api = (version: string | null, release: string | null, cb: TApiCb) =>
      agent
        .get(Paths.Versions.UpdateURL.replace(':version', version ?? '').replace(':release', release ?? ''))
        .end(apiCb(cb));

    // Success
    it(`should return a the two test URLs and a status code of "${HttpStatusCodes.OK}" if the `
      + 'request was successful.', (done) => {
        // Add spy
        const testUrls = getDummyUpdatePaths();
        jest.spyOn(VersionRepo, 'getUpdateURLs').mockResolvedValue(testUrls);
        // Call api
        api('1.0.0', 'macos', res => {
          expect(res.status).toBe(HttpStatusCodes.OK);
          if (res.body.urls) {
            for (let i = 0; i < res.body.urls.length; ++i) {
              expect(res.body.urls[i]).toMatch(testUrls[i]);
            }
          } else {
            expect(res.body).toEqual({ urls: [] });
          }
          done();
        });
      });

    // Version not found
    it('should return a JSON object with the error message of ' +
      `"${VERSION_NOT_FOUND_ERR}" and a status code of ` +
      `"${HttpStatusCodes.NOT_FOUND}" if the version or release was not found.`, (done) => {
        // Add spy
        jest.spyOn(VersionRepo, 'getUpdateURLs').mockResolvedValue(null);
        // Call api
        api('0.0.0', 'macos', res => {
          expect(res.status).toBe(HttpStatusCodes.NOT_FOUND);
          expect(res.body.error).toBe(VERSION_NOT_FOUND_ERR);
          done();
        });
      });
  });


  // Get latest versions
  describe(`"GET:${Paths.Versions.Latest}"`, () => {

    // Setup API
    const api = (cb: TApiCb) =>
      agent
        .get(Paths.Versions.Latest)
        .end(apiCb(cb));

    // Success
    it('should return a JSON object with all the versions and a status code ' +
      `of "${HttpStatusCodes.OK}" if the request was successful.`, (done) => {
        // Add spy
        const versionData = getDummyLatest();
        jest.spyOn(VersionRepo, 'getLatest').mockResolvedValue(versionData);
        // Call API
        api(res => {
          expect(res.status).toBe(HttpStatusCodes.OK);
          expect(res.body).toEqual({ latest: versionData });
          done();
        });
      });
  });


});
