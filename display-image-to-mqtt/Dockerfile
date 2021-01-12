FROM node:15-alpine
WORKDIR /usr/src/app

RUN npm install mqtt --save
RUN npm install follow-redirects --save

COPY display-image-to-mqtt.js .

CMD [ "node", "display-image-to-mqtt.js"]

EXPOSE 80/tcp

