FROM node:21-alpine

RUN apk add --no-cache \
   protoc

WORKDIR /autoupdater
COPY app/*.json /autoupdater/
RUN npm install

COPY app /autoupdater
RUN npm run build

ENV DEBUG=myapp:*

CMD ["npm", "start"]
# npm run dev to debug
