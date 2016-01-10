CREATE TABLE tab_volumes
(
    id int AUTO_INCREMENT PRIMARY KEY,
    uuid char(40) NOT NULL,
    label char(32),
    mount text,
    last_check datetime
);
