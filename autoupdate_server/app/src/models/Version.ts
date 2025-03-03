// **** Types **** //

export class Version {
  name: string;
  releases: [string] | [];

  constructor(name?: string, releases?: [string]) {
    this.name = name ?? '';
    this.releases = releases ?? [];
  }

}

