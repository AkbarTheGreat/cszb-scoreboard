import { Router } from 'express';
import jetValidator from 'jet-validator';

import Paths from '../constants/Paths';
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

apiRouter.use(Paths.Versions.Base, versionRouter);


// **** Export default **** //

export default apiRouter;
