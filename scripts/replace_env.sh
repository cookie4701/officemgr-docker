#!/bin/sh

sed -i  "s#ENV_BASEURL#$ENV_BASEURL#" /var/www/html/officemgr/app/Config/App.php
sed -i  "s#ENV_BASEURL#$ENV_BASEURL#" /var/www/html/officemgr/.env
