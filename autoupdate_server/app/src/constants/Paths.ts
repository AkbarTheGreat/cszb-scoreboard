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
  },
} as const;
