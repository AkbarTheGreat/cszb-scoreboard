// **** Types **** //

interface VersionHierarchy {
  major: number,
  minor: number,
  sub: number,
  tag: string,
}

export class Version {
  name: string;
  hierarchy: VersionHierarchy;
  releases: string[];

  constructor(name: string, releases: string[]) {
    this.name = name;
    this.hierarchy = Version.versionHierarchy(name)
    this.releases = releases;
  }


  static versionHierarchy(version: string): VersionHierarchy {
    let split: VersionHierarchy = {
      major: 0,
      minor: 0,
      sub: 0,
      tag: "",
    };
    const splits = version.split('.')
    split.major = +splits[0];
    if (splits.length > 1) {
      split.minor = +splits[1];
    }
    if (splits.length > 2) {
      const subAndTag = splits[2].split('_')
      split.sub = +subAndTag[0]
      if (subAndTag.length > 1) {
        split.tag = subAndTag[1]
      }
    }
    return split;
  }

}

