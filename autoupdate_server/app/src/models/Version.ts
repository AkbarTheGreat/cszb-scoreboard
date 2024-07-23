// **** Types **** //

export interface Version {
  name: string;
  releases: [string] | [];
}


// **** Functions **** //

/**
 * Create new Version.
 */
function new_(
  name?: string,
  releases?: [string],
): Version {
  return {
    name: (name ?? ''),
    releases: (releases ?? []),
  };
}

export default {
  new: new_,
} as const;
