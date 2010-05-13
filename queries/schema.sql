\set ON_ERROR_STOP on

DROP SCHEMA IF EXISTS test_data CASCADE;
DROP SCHEMA IF EXISTS test_view CASCADE;
CREATE SCHEMA test_data;
CREATE SCHEMA test_view;
SET SEARCH_PATH = test_data, test_view;

CREATE TABLE proband (
       proband_id bigserial PRIMARY KEY
);

CREATE TABLE sample (
       sample_id bigserial PRIMARY KEY
);


CREATE TABLE proband__sample (
       proband_id bigint NOT NULL REFERENCES proband,
       sample_id bigint NOT NULL REFERENCES sample,

       UNIQUE(proband_id, sample_id)
);

CREATE TABLE project (
      project_id bigserial PRIMARY KEY,
      name text NOT NULL UNIQUE
);

/*
 * Stuff like:
 *     -Double Entry
 *     -Single Entry
 *     -Machine Read
 *     - ...
 */
CREATE TABLE data_quality (
       data_quality_id bigserial PRIMARY KEY,
       name text NOT NULL UNIQUE,
       description text
);


CREATE TABLE information_set (
       information_set_id bigserial PRIMARY KEY,
       name text NOT NULL UNIQUE,
       description text
);

CREATE TABLE information (
       information_id bigserial PRIMARY KEY,
       information_set_id bigint NOT NULL REFERENCES information_set,
       name text NOT NULL UNIQUE,
       description text
);
CREATE INDEX information__information_set_id ON information (information_set_id);



CREATE TABLE information_set_instance (
       information_set_instance_id bigserial PRIMARY KEY,
       information_set_id bigint NOT NULL REFERENCES information_set
);
CREATE INDEX information_set_instance__information_set_id ON information_set_instance (information_set_id);

CREATE TABLE information_set_instance__proband (
       information_set_instance_id bigint NOT NULL REFERENCES information_set_instance,
       proband_id bigint NOT NULL REFERENCES proband,
       UNIQUE (information_set_instance_id, proband_id)
);
CREATE INDEX information_set_instance__proband__information_set_id ON information_set_instance__proband (information_set_instance_id);
CREATE INDEX information_set_instance__proband__proband_id ON information_set_instance__proband (proband_id);

CREATE TABLE information_set_instance__sample (
       information_set_instance_id bigint NOT NULL REFERENCES information_set_instance,
       sample_id bigint NOT NULL REFERENCES sample,
       UNIQUE (information_set_instance_id, sample_id)
);

CREATE INDEX information_set_instance__sample__information_set_id ON information_set_instance__sample (information_set_instance_id);
CREATE INDEX information_set_instance__sample__sample_id ON information_set_instance__sample (sample_id);

CREATE TABLE information_instance (
       information_instance_id bigserial PRIMARY KEY,
       information_set_instance_id bigint NOT NULL REFERENCES information_set_instance,
       information_id bigint NOT NULL REFERENCES information,
       data_quality_id int REFERENCES data_quality
);
CREATE INDEX information_instance__information_set_instance_id ON information_set_instance(information_set_instance_id);
CREATE INDEX information_instance__information_id ON information(information_id);

CREATE TABLE information_about_tnm (
       information_about_tnm bigserial PRIMARY KEY,
       information_instance_id bigint REFERENCES information_instance,
       t int,
       n int,
       m int
);
CREATE INDEX information_about_tnm__information_instance_id ON information_about_tnm(information_instance_id);


CREATE TABLE information_about_sequenced_data (
       information_about_sequenced_data bigserial PRIMARY KEY,
       information_instance_id bigint REFERENCES information_instance,
       filename text,
       data text
);
CREATE INDEX information_about_sequenced_data__information_instance_id ON information_about_sequenced_data(information_instance_id);


CREATE TABLE information_about_time (
       information_about_sequenced_data bigserial PRIMARY KEY,
       information_instance_id bigint REFERENCES information_instance,
       data timestamp
);
CREATE INDEX information_about_time__information_instance_id ON information_about_time(information_instance_id);

CREATE TABLE information_about_text (
       information_about_text_data bigserial PRIMARY KEY,
       information_instance_id bigint REFERENCES information_instance,
       data text
);
CREATE INDEX information_about_text__information_instance_id ON information_about_text(information_instance_id);

/*
 * Normally refers to a table containing icd10 classiffication codes
 */
CREATE TABLE information_about_icd10_classification (
       information_about_icd10_classification_data bigserial PRIMARY KEY,
       information_instance_id bigint REFERENCES information_instance,
       data int
);
CREATE INDEX information_about_icd10_classification__information_instance_id ON information_about_icd10_classification(information_instance_id);

CREATE TABLE information_about_location (
       information_about_allowance bigserial PRIMARY KEY,
       information_instance_id bigint REFERENCES information_instance,
       street text,
       building text
       -- Additional Attributes
);
CREATE INDEX information_about_location__information_instance_id ON information_about_location(information_instance_id);


CREATE TABLE information_about_allowance (
       information_about_allowance bigserial PRIMARY KEY,
       information_instance_id bigint REFERENCES information_instance,
       granted bool
);
CREATE INDEX information_about_allowance__information_instance_id ON information_about_allowance(information_instance_id);

CREATE TABLE information_about_clinic (
       information_about_clinic bigserial PRIMARY KEY,
       information_instance_id bigint REFERENCES information_instance,
       clinic_id int
);
CREATE INDEX information_about_clinic__information_instance_id ON information_about_allowance(information_instance_id);

CREATE TABLE information_about_personel (
       information_about_personel bigserial PRIMARY KEY,
       information_instance_id bigint REFERENCES information_instance,
       name text
);
CREATE INDEX information_about_personel__information_instance_id ON information_about_allowance(information_instance_id);

/*
 * Lots of other
 * information_about_* tables
 */

CREATE TABLE information_about_placeholder (
       information_about_personel bigserial PRIMARY KEY,
       information_instance_id bigint REFERENCES information_instance,
       data text
);
CREATE INDEX information_about_placeholder__information_instance_id ON information_about_allowance(information_instance_id);



