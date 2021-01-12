FROM node:12
WORKDIR /usr/src/app

RUN npm install mqtt --save

COPY epd-loader.js .

CMD [ "node", "epd-loader.js"]