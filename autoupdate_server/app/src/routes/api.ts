import express, { Router } from 'express';
import jetValidator from 'jet-validator';

import Filesystem from '@src/constants/Filesystem';
import Paths from '@src/constants/Paths';
import VersionRoutes from './VersionRoutes';


// **** Variables **** //

const apiRouter = Router(),
  validate = jetValidator();


// ** Add VersionRouter ** //

const versionRouter = Router();

versionRouter.get(
  Paths.Versions.All,
  VersionRoutes.getAll,
);

versionRouter.get(
  Paths.Versions.Latest,
  VersionRoutes.getLatest,
);

versionRouter.get(
  Paths.Versions.Info,
  VersionRoutes.getInfo,
);

versionRouter.get(
  Paths.Versions.UpdateURL,
  VersionRoutes.getUpdateURLs,
);

apiRouter.use(Paths.Versions.Base, versionRouter);

apiRouter.use(Paths.Update.Data, express.static(Filesystem.ReleaseData));


// **** Export default **** //

export default apiRouter;
