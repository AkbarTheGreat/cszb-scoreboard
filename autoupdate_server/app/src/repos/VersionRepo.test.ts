import VersionRepo, { _versionSorter } from './VersionRepo';
import { Version } from '@src/models/Version';

test('Gets all expected versions from test repo', async () => {
  const data = await VersionRepo.getAll();
  expect(data).toEqual([
    '0.5.0',
    '0.5.0_hotfix',
    '0.5.0_hotfox',
    '0.10.0',
    '1.0.1',
  ]);
});

test('Get info returns expected value for known version', async () => {
  const version = await VersionRepo.getInfo('0.5.0');
  expect(version).toEqual(
    new Version('0.5.0', ['win64'])
  );
});

test('Get info returns null for invalid version', async () => {
  const version = await VersionRepo.getInfo('9.9.9');
  expect(version).toEqual(null);
});

test('Get latest returns the highest version number', async () => {
  const latest = await VersionRepo.getLatest();
  expect(latest).toEqual(new Version('1.0.1', ['macos', 'win64']));
});

test('Version sort works as expected', async () => {
  expect(_versionSorter).not.toBeNull()
  if (_versionSorter) {
    // Check baseline comparisons
    expect(_versionSorter('0.9.9', '1.0.0')).toEqual(-1);
    expect(_versionSorter('1.0.0', '1.0.0')).toEqual(0);
    expect(_versionSorter('1.0.0', '0.9.9')).toEqual(1);
    // Check natural ordering and minor revision differences
    expect(_versionSorter('0.9.9', '0.10.0')).toEqual(-1);
    expect(_versionSorter('0.10.0', '0.9.9')).toEqual(1);
    // Check sub-version differences
    expect(_versionSorter('0.9.0', '0.9.9')).toEqual(-1);
    expect(_versionSorter('0.9.9', '0.9.0')).toEqual(1);
    // Check that trailing strings are sorted appropriately
    expect(_versionSorter('1.0.0', '1.0.0_hotfix')).toEqual(-1);
    expect(_versionSorter('1.0.0_hitfix', '1.0.0_hotfix')).toEqual(-1);
    expect(_versionSorter('1.0.0_hotfix', '1.0.0')).toEqual(1);
    expect(_versionSorter('1.0.0_hotfix', '1.0.0_hitfix')).toEqual(1);
  }
});
