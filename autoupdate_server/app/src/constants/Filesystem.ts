import EnvVars from '@src/constants/EnvVars';

export default {
  ReleaseData: EnvVars.NodeEnv == 'test' ? '../testdata/releases/' : '/releases/',
} as const;
