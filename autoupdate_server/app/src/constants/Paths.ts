/**
 * Express router paths go here.
 */


export default {
  Base: '/api',
  Versions: {
    Base: '/versions',
    All: '/all',
    Latest: '/latest',
    Info: '/info/:version',
    UpdateURL: '/update_path/:version/:release',
  },
  Update: {
    Data: '/update_data',
  },
} as const;
