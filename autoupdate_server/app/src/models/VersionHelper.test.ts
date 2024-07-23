import VersionHelper from './VersionHelper';

test('version hierarchy simple parsing test', () => {
  expect(VersionHelper.buildVersionHierarchy('1.0.1')).toEqual({ major: 1, minor: 0, sub: 1, tag: '' });
  expect(VersionHelper.buildVersionHierarchy('0.10.0')).toEqual({ major: 0, minor: 10, sub: 0, tag: '' });
  expect(VersionHelper.buildVersionHierarchy('1.1')).toEqual({ major: 1, minor: 1, sub: 0, tag: '' });
  expect(VersionHelper.buildVersionHierarchy('1')).toEqual({ major: 1, minor: 0, sub: 0, tag: '' });
});

test('version hierarchy tag parsing', () => {
  expect(VersionHelper.buildVersionHierarchy('1.0.1_foo')).toEqual({ major: 1, minor: 0, sub: 1, tag: 'foo' });
  expect(VersionHelper.buildVersionHierarchy('0.0.1_1')).toEqual({ major: 0, minor: 0, sub: 1, tag: '1' });
  expect(VersionHelper.buildVersionHierarchy('0.0.1_')).toEqual({ major: 0, minor: 0, sub: 1, tag: '' });
});

test('invalid version hierarchy parsing', () => {
  expect(VersionHelper.buildVersionHierarchy('0.1._foo')).toEqual({ major: 0, minor: 1, sub: 0, tag: 'foo' });
  expect(VersionHelper.buildVersionHierarchy('0.1_foo')).toEqual({ major: 0, minor: NaN, sub: 0, tag: '' });
});
