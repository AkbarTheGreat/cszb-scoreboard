// **** Types **** //

import { Version, VersionHierarchy } from '@proto/version_info'

export default class VersionHelper {
  static buildVersionHierarchy(versionNumber: string): VersionHierarchy {
    let split: VersionHierarchy = { major: 0, minor: 0, sub: 0, tag: '' };
    const splits = versionNumber.split('.')
    split.major = +splits[0];
    if (splits.length > 1) {
      split.minor = +splits[1];
    }
    if (splits.length > 2) {
      const subAndTag = splits[2].split('_');
      split.sub = +subAndTag[0];
      if (subAndTag.length > 1) {
        split.tag = subAndTag[1];
      }
    }
    return split;
  }

  static buildVersion(name: string, releases?: string[]): Version {
    return { name: name, hierarchy: this.buildVersionHierarchy(name), releases: releases ? releases : [] }
  }
}

