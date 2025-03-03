import { Version } from './Version';

test('version hierarchy simple parsing test', () => {
  expect(Version.versionHierarchy('1.0.1')).toEqual({major: 1, minor: 0, sub: 1, tag: ''});
  expect(Version.versionHierarchy('0.10.0')).toEqual({major: 0, minor: 10, sub: 0, tag: ''});
  expect(Version.versionHierarchy('1.1')).toEqual({major: 1, minor: 1, sub: 0, tag: ''});
  expect(Version.versionHierarchy('1')).toEqual({major: 1, minor: 0, sub: 0, tag: ''});
});

test('version hierarchy tag parsing', () => {
  expect(Version.versionHierarchy('1.0.1_foo')).toEqual({major: 1, minor: 0, sub: 1, tag: 'foo'});
  expect(Version.versionHierarchy('0.0.1_1')).toEqual({major: 0, minor: 0, sub: 1, tag: '1'});
  expect(Version.versionHierarchy('0.0.1_')).toEqual({major: 0, minor: 0, sub: 1, tag: ''});
});

test('invalid version hierarchy parsing', () => {
  expect(Version.versionHierarchy('0.1._foo')).toEqual({major: 0, minor: 1, sub: 0, tag: 'foo'});
  expect(Version.versionHierarchy('0.1_foo')).toEqual({major: 0, minor: NaN, sub: 0, tag: ''});
});
