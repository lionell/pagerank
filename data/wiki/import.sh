# https://hub.docker.com/_/mariadb/
# https://hub.docker.com/r/phpmyadmin/phpmyadmin/

mysql -u root -p -e 'create database wiki;'
zcat enwiki-latest-pagelinks.sql.gz | mysql -u root -p wiki
zcat enwiki-latest-page.sql.gz | mysql -u root -p wiki
