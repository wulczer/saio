--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = public, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: bar1; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bar1 (
    id integer NOT NULL,
    name character varying NOT NULL,
    bletch1_id integer NOT NULL,
    bar2_id integer NOT NULL,
    bar3_id integer NOT NULL,
    bar4_id integer NOT NULL,
    bar5_id integer NOT NULL,
    bar6_id integer NOT NULL,
    bar7_id integer NOT NULL,
    bar8_id integer,
    bar9_id integer,
    bar10_id integer,
    bar11_id integer,
    bar12_id integer,
    bar13_id integer,
    bar14_id integer
);


ALTER TABLE public.bar1 OWNER TO rhaas;

--
-- Name: bar10; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bar10 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.bar10 OWNER TO rhaas;

--
-- Name: bar10_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bar10_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bar10_id_seq OWNER TO rhaas;

--
-- Name: bar10_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bar10_id_seq OWNED BY bar10.id;


--
-- Name: bar11; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bar11 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.bar11 OWNER TO rhaas;

--
-- Name: bar11_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bar11_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bar11_id_seq OWNER TO rhaas;

--
-- Name: bar11_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bar11_id_seq OWNED BY bar11.id;


--
-- Name: bar12; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bar12 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.bar12 OWNER TO rhaas;

--
-- Name: bar12_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bar12_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bar12_id_seq OWNER TO rhaas;

--
-- Name: bar12_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bar12_id_seq OWNED BY bar12.id;


--
-- Name: bar13; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bar13 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.bar13 OWNER TO rhaas;

--
-- Name: bar13_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bar13_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bar13_id_seq OWNER TO rhaas;

--
-- Name: bar13_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bar13_id_seq OWNED BY bar13.id;


--
-- Name: bar14; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bar14 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.bar14 OWNER TO rhaas;

--
-- Name: bar14_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bar14_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bar14_id_seq OWNER TO rhaas;

--
-- Name: bar14_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bar14_id_seq OWNED BY bar14.id;


--
-- Name: bar1_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bar1_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bar1_id_seq OWNER TO rhaas;

--
-- Name: bar1_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bar1_id_seq OWNED BY bar1.id;


--
-- Name: bar2; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bar2 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.bar2 OWNER TO rhaas;

--
-- Name: bar2_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bar2_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bar2_id_seq OWNER TO rhaas;

--
-- Name: bar2_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bar2_id_seq OWNED BY bar2.id;


--
-- Name: bar3; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bar3 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.bar3 OWNER TO rhaas;

--
-- Name: bar3_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bar3_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bar3_id_seq OWNER TO rhaas;

--
-- Name: bar3_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bar3_id_seq OWNED BY bar3.id;


--
-- Name: bar4; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bar4 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.bar4 OWNER TO rhaas;

--
-- Name: bar4_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bar4_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bar4_id_seq OWNER TO rhaas;

--
-- Name: bar4_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bar4_id_seq OWNED BY bar4.id;


--
-- Name: bar5; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bar5 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.bar5 OWNER TO rhaas;

--
-- Name: bar5_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bar5_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bar5_id_seq OWNER TO rhaas;

--
-- Name: bar5_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bar5_id_seq OWNED BY bar5.id;


--
-- Name: bar6; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bar6 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.bar6 OWNER TO rhaas;

--
-- Name: bar6_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bar6_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bar6_id_seq OWNER TO rhaas;

--
-- Name: bar6_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bar6_id_seq OWNED BY bar6.id;


--
-- Name: bar7; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bar7 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.bar7 OWNER TO rhaas;

--
-- Name: bar7_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bar7_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bar7_id_seq OWNER TO rhaas;

--
-- Name: bar7_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bar7_id_seq OWNED BY bar7.id;


--
-- Name: bar8; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bar8 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.bar8 OWNER TO rhaas;

--
-- Name: bar8_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bar8_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bar8_id_seq OWNER TO rhaas;

--
-- Name: bar8_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bar8_id_seq OWNED BY bar8.id;


--
-- Name: bar9; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bar9 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.bar9 OWNER TO rhaas;

--
-- Name: bar9_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bar9_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bar9_id_seq OWNER TO rhaas;

--
-- Name: bar9_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bar9_id_seq OWNED BY bar9.id;


--
-- Name: bletch1; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bletch1 (
    id integer NOT NULL,
    name character varying NOT NULL,
    bletch2_id integer NOT NULL,
    bletch3_id integer,
    bletch4_id integer,
    bletch5_id integer,
    bletch6_id integer,
    bletch7_id integer,
    bletch8_id integer,
    bletch9_id integer
);


ALTER TABLE public.bletch1 OWNER TO rhaas;

--
-- Name: bletch2; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bletch2 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.bletch2 OWNER TO rhaas;

--
-- Name: bletch3; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bletch3 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.bletch3 OWNER TO rhaas;

--
-- Name: bletch4; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bletch4 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.bletch4 OWNER TO rhaas;

--
-- Name: bletch5; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bletch5 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.bletch5 OWNER TO rhaas;

--
-- Name: bletch6; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bletch6 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.bletch6 OWNER TO rhaas;

--
-- Name: bletch7; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bletch7 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.bletch7 OWNER TO rhaas;

--
-- Name: bletch8; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bletch8 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.bletch8 OWNER TO rhaas;

--
-- Name: bletch9; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE bletch9 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.bletch9 OWNER TO rhaas;

--
-- Name: bletch_view; Type: VIEW; Schema: public; Owner: rhaas
--

CREATE VIEW bletch_view AS
    SELECT bletch1.id, bletch1.name, bletch1.bletch2_id, bletch1.bletch3_id, bletch1.bletch4_id, bletch1.bletch5_id, bletch1.bletch6_id, bletch1.bletch7_id, bletch1.bletch8_id, bletch1.bletch9_id, bletch2.name AS bletch2, bletch3.name AS bletch3, bletch4.name AS bletch4, bletch5.name AS bletch5, bletch6.name AS bletch6, bletch7.name AS bletch7, bletch8.name AS bletch8, bletch9.name AS bletch9 FROM bletch2, (((((((bletch1 LEFT JOIN bletch3 ON ((bletch1.bletch3_id = bletch3.id))) LEFT JOIN bletch4 ON ((bletch1.bletch4_id = bletch4.id))) LEFT JOIN bletch5 ON ((bletch1.bletch5_id = bletch5.id))) LEFT JOIN bletch6 ON ((bletch1.bletch6_id = bletch6.id))) LEFT JOIN bletch7 ON ((bletch1.bletch7_id = bletch7.id))) LEFT JOIN bletch8 ON ((bletch1.bletch8_id = bletch8.id))) LEFT JOIN bletch9 ON ((bletch1.bletch9_id = bletch9.id))) WHERE (bletch1.bletch2_id = bletch2.id);


ALTER TABLE public.bletch_view OWNER TO rhaas;

--
-- Name: bar_view; Type: VIEW; Schema: public; Owner: rhaas
--

CREATE VIEW bar_view AS
    SELECT bar1.id, bar1.name, bar1.bletch1_id, bar1.bar2_id, bar1.bar3_id, bar1.bar4_id, bar1.bar5_id, bar1.bar6_id, bar1.bar7_id, bar1.bar8_id, bar1.bar9_id, bar1.bar10_id, bar1.bar11_id, bar1.bar12_id, bar1.bar13_id, bar1.bar14_id, bar2.name AS bar2, bar3.name AS bar3, bar4.name AS bar4, bar5.name AS bar5, bar6.name AS bar6, bar7.name AS bar7, bar8.name AS bar8, bar9.name AS bar9, bar10.name AS bar10, bar11.name AS bar11, bar12.name AS bar12, bar13.name AS bar13, bar14.name AS bar14, bletch_view.name AS bletch1, bletch_view.bletch2, bletch_view.bletch3, bletch_view.bletch4, bletch_view.bletch5, bletch_view.bletch6, bletch_view.bletch7, bletch_view.bletch8, bletch_view.bletch9 FROM bar2, bar3, bar4, bar5, bar6, bar7, bletch_view, (((((((bar1 LEFT JOIN bar8 ON ((bar1.bar8_id = bar8.id))) LEFT JOIN bar9 ON ((bar1.bar9_id = bar9.id))) LEFT JOIN bar10 ON ((bar1.bar10_id = bar10.id))) LEFT JOIN bar11 ON ((bar1.bar11_id = bar11.id))) LEFT JOIN bar12 ON ((bar1.bar12_id = bar12.id))) LEFT JOIN bar13 ON ((bar1.bar13_id = bar13.id))) LEFT JOIN bar14 ON ((bar1.bar14_id = bar14.id))) WHERE (((((((bar1.bar2_id = bar2.id) AND (bar1.bar3_id = bar3.id)) AND (bar1.bar4_id = bar4.id)) AND (bar1.bar5_id = bar5.id)) AND (bar1.bar6_id = bar6.id)) AND (bar1.bar7_id = bar7.id)) AND (bar1.bletch1_id = bletch_view.id));


ALTER TABLE public.bar_view OWNER TO rhaas;

--
-- Name: baz1; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE baz1 (
    id integer NOT NULL,
    name character varying NOT NULL,
    baz2_id integer NOT NULL,
    baz3_id integer NOT NULL,
    baz4_id integer NOT NULL,
    baz5_id integer,
    baz6_id integer,
    baz7_id integer,
    baz8_id integer,
    baz9_id integer
);


ALTER TABLE public.baz1 OWNER TO rhaas;

--
-- Name: baz1_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE baz1_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.baz1_id_seq OWNER TO rhaas;

--
-- Name: baz1_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE baz1_id_seq OWNED BY baz1.id;


--
-- Name: baz2; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE baz2 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.baz2 OWNER TO rhaas;

--
-- Name: baz2_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE baz2_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.baz2_id_seq OWNER TO rhaas;

--
-- Name: baz2_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE baz2_id_seq OWNED BY baz2.id;


--
-- Name: baz3; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE baz3 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.baz3 OWNER TO rhaas;

--
-- Name: baz3_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE baz3_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.baz3_id_seq OWNER TO rhaas;

--
-- Name: baz3_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE baz3_id_seq OWNED BY baz3.id;


--
-- Name: baz4; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE baz4 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.baz4 OWNER TO rhaas;

--
-- Name: baz4_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE baz4_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.baz4_id_seq OWNER TO rhaas;

--
-- Name: baz4_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE baz4_id_seq OWNED BY baz4.id;


--
-- Name: baz5; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE baz5 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.baz5 OWNER TO rhaas;

--
-- Name: baz5_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE baz5_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.baz5_id_seq OWNER TO rhaas;

--
-- Name: baz5_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE baz5_id_seq OWNED BY baz5.id;


--
-- Name: baz6; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE baz6 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.baz6 OWNER TO rhaas;

--
-- Name: baz6_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE baz6_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.baz6_id_seq OWNER TO rhaas;

--
-- Name: baz6_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE baz6_id_seq OWNED BY baz6.id;


--
-- Name: baz7; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE baz7 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.baz7 OWNER TO rhaas;

--
-- Name: baz7_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE baz7_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.baz7_id_seq OWNER TO rhaas;

--
-- Name: baz7_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE baz7_id_seq OWNED BY baz7.id;


--
-- Name: baz8; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE baz8 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.baz8 OWNER TO rhaas;

--
-- Name: baz8_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE baz8_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.baz8_id_seq OWNER TO rhaas;

--
-- Name: baz8_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE baz8_id_seq OWNED BY baz8.id;


--
-- Name: baz9; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE baz9 (
    id integer NOT NULL,
    name character varying NOT NULL
);


ALTER TABLE public.baz9 OWNER TO rhaas;

--
-- Name: baz9_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE baz9_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.baz9_id_seq OWNER TO rhaas;

--
-- Name: baz9_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE baz9_id_seq OWNED BY baz9.id;


--
-- Name: baz_view; Type: VIEW; Schema: public; Owner: rhaas
--

CREATE VIEW baz_view AS
    SELECT baz1.id, baz1.name, baz1.baz2_id, baz1.baz3_id, baz1.baz4_id, baz1.baz5_id, baz1.baz6_id, baz1.baz7_id, baz1.baz8_id, baz1.baz9_id, baz2.name AS baz2, baz3.name AS baz3, baz4.name AS baz4, baz5.name AS baz5, baz6.name AS baz6, baz7.name AS baz7, baz8.name AS baz8, baz9.name AS baz9 FROM baz2, baz3, ((((((baz1 LEFT JOIN baz4 ON ((baz1.baz4_id = baz4.id))) LEFT JOIN baz5 ON ((baz1.baz5_id = baz5.id))) LEFT JOIN baz6 ON ((baz1.baz6_id = baz6.id))) LEFT JOIN baz7 ON ((baz1.baz7_id = baz7.id))) LEFT JOIN baz8 ON ((baz1.baz8_id = baz8.id))) LEFT JOIN baz9 ON ((baz1.baz9_id = baz9.id))) WHERE ((baz1.baz2_id = baz2.id) AND (baz1.baz3_id = baz3.id));


ALTER TABLE public.baz_view OWNER TO rhaas;

--
-- Name: bletch1_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bletch1_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bletch1_id_seq OWNER TO rhaas;

--
-- Name: bletch1_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bletch1_id_seq OWNED BY bletch1.id;


--
-- Name: bletch2_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bletch2_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bletch2_id_seq OWNER TO rhaas;

--
-- Name: bletch2_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bletch2_id_seq OWNED BY bletch2.id;


--
-- Name: bletch3_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bletch3_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bletch3_id_seq OWNER TO rhaas;

--
-- Name: bletch3_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bletch3_id_seq OWNED BY bletch3.id;


--
-- Name: bletch4_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bletch4_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bletch4_id_seq OWNER TO rhaas;

--
-- Name: bletch4_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bletch4_id_seq OWNED BY bletch4.id;


--
-- Name: bletch5_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bletch5_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bletch5_id_seq OWNER TO rhaas;

--
-- Name: bletch5_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bletch5_id_seq OWNED BY bletch5.id;


--
-- Name: bletch6_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bletch6_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bletch6_id_seq OWNER TO rhaas;

--
-- Name: bletch6_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bletch6_id_seq OWNED BY bletch6.id;


--
-- Name: bletch7_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bletch7_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bletch7_id_seq OWNER TO rhaas;

--
-- Name: bletch7_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bletch7_id_seq OWNED BY bletch7.id;


--
-- Name: bletch8_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bletch8_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bletch8_id_seq OWNER TO rhaas;

--
-- Name: bletch8_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bletch8_id_seq OWNED BY bletch8.id;


--
-- Name: bletch9_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE bletch9_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.bletch9_id_seq OWNER TO rhaas;

--
-- Name: bletch9_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE bletch9_id_seq OWNED BY bletch9.id;


--
-- Name: foo1; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE foo1 (
    id integer NOT NULL,
    name character varying NOT NULL,
    bar1_id integer NOT NULL,
    baz1_id integer NOT NULL,
    foo2_id integer NOT NULL,
    foo3_id integer NOT NULL,
    foo4_id integer,
    foo5_id integer,
    foo6_id integer
);


ALTER TABLE public.foo1 OWNER TO rhaas;

--
-- Name: foo1_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE foo1_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.foo1_id_seq OWNER TO rhaas;

--
-- Name: foo1_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE foo1_id_seq OWNED BY foo1.id;


--
-- Name: foo2; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE foo2 (
    id integer NOT NULL,
    name character varying
);


ALTER TABLE public.foo2 OWNER TO rhaas;

--
-- Name: foo2_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE foo2_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.foo2_id_seq OWNER TO rhaas;

--
-- Name: foo2_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE foo2_id_seq OWNED BY foo2.id;


--
-- Name: foo3; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE foo3 (
    id integer NOT NULL,
    name character varying
);


ALTER TABLE public.foo3 OWNER TO rhaas;

--
-- Name: foo3_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE foo3_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.foo3_id_seq OWNER TO rhaas;

--
-- Name: foo3_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE foo3_id_seq OWNED BY foo3.id;


--
-- Name: foo4; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE foo4 (
    id integer NOT NULL,
    name character varying
);


ALTER TABLE public.foo4 OWNER TO rhaas;

--
-- Name: foo4_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE foo4_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.foo4_id_seq OWNER TO rhaas;

--
-- Name: foo4_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE foo4_id_seq OWNED BY foo4.id;


--
-- Name: foo5; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE foo5 (
    id integer NOT NULL,
    name character varying
);


ALTER TABLE public.foo5 OWNER TO rhaas;

--
-- Name: foo5_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE foo5_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.foo5_id_seq OWNER TO rhaas;

--
-- Name: foo5_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE foo5_id_seq OWNED BY foo5.id;


--
-- Name: foo6; Type: TABLE; Schema: public; Owner: rhaas; Tablespace: 
--

CREATE TABLE foo6 (
    id integer NOT NULL,
    name character varying
);


ALTER TABLE public.foo6 OWNER TO rhaas;

--
-- Name: foo6_id_seq; Type: SEQUENCE; Schema: public; Owner: rhaas
--

CREATE SEQUENCE foo6_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.foo6_id_seq OWNER TO rhaas;

--
-- Name: foo6_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: rhaas
--

ALTER SEQUENCE foo6_id_seq OWNED BY foo6.id;


--
-- Name: foo_view; Type: VIEW; Schema: public; Owner: rhaas
--

CREATE VIEW foo_view AS
    SELECT foo1.id, foo1.name, foo1.bar1_id, foo1.baz1_id, foo1.foo2_id, foo1.foo3_id, foo1.foo4_id, foo1.foo5_id, foo1.foo6_id, bar_view.name AS bar1, bar_view.bar2, bar_view.bar3, bar_view.bar4, bar_view.bar5, bar_view.bar6, bar_view.bar7, bar_view.bar8, bar_view.bar9, bar_view.bar10, bar_view.bar11, bar_view.bar12, bar_view.bar13, bar_view.bar14, baz_view.name AS baz1, baz_view.baz2, baz_view.baz3, baz_view.baz4, baz_view.baz5, baz_view.baz6, baz_view.baz7, baz_view.baz8, baz_view.baz9, bar_view.bletch1, bar_view.bletch2, bar_view.bletch3, bar_view.bletch4, bar_view.bletch5, bar_view.bletch6, bar_view.bletch7, bar_view.bletch8, bar_view.bletch9 FROM ((((((foo1 JOIN bar_view ON ((foo1.bar1_id = bar_view.id))) JOIN baz_view ON ((foo1.baz1_id = baz_view.id))) JOIN foo3 ON ((foo1.foo3_id = foo3.id))) LEFT JOIN foo4 ON ((foo1.foo4_id = foo4.id))) LEFT JOIN foo5 ON ((foo1.foo5_id = foo5.id))) LEFT JOIN foo6 ON ((foo1.foo6_id = foo6.id)));


ALTER TABLE public.foo_view OWNER TO rhaas;

--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bar1 ALTER COLUMN id SET DEFAULT nextval('bar1_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bar10 ALTER COLUMN id SET DEFAULT nextval('bar10_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bar11 ALTER COLUMN id SET DEFAULT nextval('bar11_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bar12 ALTER COLUMN id SET DEFAULT nextval('bar12_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bar13 ALTER COLUMN id SET DEFAULT nextval('bar13_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bar14 ALTER COLUMN id SET DEFAULT nextval('bar14_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bar2 ALTER COLUMN id SET DEFAULT nextval('bar2_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bar3 ALTER COLUMN id SET DEFAULT nextval('bar3_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bar4 ALTER COLUMN id SET DEFAULT nextval('bar4_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bar5 ALTER COLUMN id SET DEFAULT nextval('bar5_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bar6 ALTER COLUMN id SET DEFAULT nextval('bar6_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bar7 ALTER COLUMN id SET DEFAULT nextval('bar7_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bar8 ALTER COLUMN id SET DEFAULT nextval('bar8_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bar9 ALTER COLUMN id SET DEFAULT nextval('bar9_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE baz1 ALTER COLUMN id SET DEFAULT nextval('baz1_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE baz2 ALTER COLUMN id SET DEFAULT nextval('baz2_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE baz3 ALTER COLUMN id SET DEFAULT nextval('baz3_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE baz4 ALTER COLUMN id SET DEFAULT nextval('baz4_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE baz5 ALTER COLUMN id SET DEFAULT nextval('baz5_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE baz6 ALTER COLUMN id SET DEFAULT nextval('baz6_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE baz7 ALTER COLUMN id SET DEFAULT nextval('baz7_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE baz8 ALTER COLUMN id SET DEFAULT nextval('baz8_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE baz9 ALTER COLUMN id SET DEFAULT nextval('baz9_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bletch1 ALTER COLUMN id SET DEFAULT nextval('bletch1_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bletch2 ALTER COLUMN id SET DEFAULT nextval('bletch2_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bletch3 ALTER COLUMN id SET DEFAULT nextval('bletch3_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bletch4 ALTER COLUMN id SET DEFAULT nextval('bletch4_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bletch5 ALTER COLUMN id SET DEFAULT nextval('bletch5_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bletch6 ALTER COLUMN id SET DEFAULT nextval('bletch6_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bletch7 ALTER COLUMN id SET DEFAULT nextval('bletch7_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bletch8 ALTER COLUMN id SET DEFAULT nextval('bletch8_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE bletch9 ALTER COLUMN id SET DEFAULT nextval('bletch9_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE foo1 ALTER COLUMN id SET DEFAULT nextval('foo1_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE foo2 ALTER COLUMN id SET DEFAULT nextval('foo2_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE foo3 ALTER COLUMN id SET DEFAULT nextval('foo3_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE foo4 ALTER COLUMN id SET DEFAULT nextval('foo4_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE foo5 ALTER COLUMN id SET DEFAULT nextval('foo5_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: rhaas
--

ALTER TABLE foo6 ALTER COLUMN id SET DEFAULT nextval('foo6_id_seq'::regclass);


--
-- Name: bar10_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bar10
    ADD CONSTRAINT bar10_pkey PRIMARY KEY (id);


--
-- Name: bar11_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bar11
    ADD CONSTRAINT bar11_pkey PRIMARY KEY (id);


--
-- Name: bar12_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bar12
    ADD CONSTRAINT bar12_pkey PRIMARY KEY (id);


--
-- Name: bar13_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bar13
    ADD CONSTRAINT bar13_pkey PRIMARY KEY (id);


--
-- Name: bar14_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bar14
    ADD CONSTRAINT bar14_pkey PRIMARY KEY (id);


--
-- Name: bar1_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bar1
    ADD CONSTRAINT bar1_pkey PRIMARY KEY (id);


--
-- Name: bar2_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bar2
    ADD CONSTRAINT bar2_pkey PRIMARY KEY (id);


--
-- Name: bar3_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bar3
    ADD CONSTRAINT bar3_pkey PRIMARY KEY (id);


--
-- Name: bar4_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bar4
    ADD CONSTRAINT bar4_pkey PRIMARY KEY (id);


--
-- Name: bar5_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bar5
    ADD CONSTRAINT bar5_pkey PRIMARY KEY (id);


--
-- Name: bar6_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bar6
    ADD CONSTRAINT bar6_pkey PRIMARY KEY (id);


--
-- Name: bar7_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bar7
    ADD CONSTRAINT bar7_pkey PRIMARY KEY (id);


--
-- Name: bar8_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bar8
    ADD CONSTRAINT bar8_pkey PRIMARY KEY (id);


--
-- Name: bar9_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bar9
    ADD CONSTRAINT bar9_pkey PRIMARY KEY (id);


--
-- Name: baz1_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY baz1
    ADD CONSTRAINT baz1_pkey PRIMARY KEY (id);


--
-- Name: baz2_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY baz2
    ADD CONSTRAINT baz2_pkey PRIMARY KEY (id);


--
-- Name: baz3_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY baz3
    ADD CONSTRAINT baz3_pkey PRIMARY KEY (id);


--
-- Name: baz4_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY baz4
    ADD CONSTRAINT baz4_pkey PRIMARY KEY (id);


--
-- Name: baz5_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY baz5
    ADD CONSTRAINT baz5_pkey PRIMARY KEY (id);


--
-- Name: baz6_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY baz6
    ADD CONSTRAINT baz6_pkey PRIMARY KEY (id);


--
-- Name: baz7_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY baz7
    ADD CONSTRAINT baz7_pkey PRIMARY KEY (id);


--
-- Name: baz8_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY baz8
    ADD CONSTRAINT baz8_pkey PRIMARY KEY (id);


--
-- Name: baz9_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY baz9
    ADD CONSTRAINT baz9_pkey PRIMARY KEY (id);


--
-- Name: bletch1_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bletch1
    ADD CONSTRAINT bletch1_pkey PRIMARY KEY (id);


--
-- Name: bletch2_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bletch2
    ADD CONSTRAINT bletch2_pkey PRIMARY KEY (id);


--
-- Name: bletch3_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bletch3
    ADD CONSTRAINT bletch3_pkey PRIMARY KEY (id);


--
-- Name: bletch4_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bletch4
    ADD CONSTRAINT bletch4_pkey PRIMARY KEY (id);


--
-- Name: bletch5_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bletch5
    ADD CONSTRAINT bletch5_pkey PRIMARY KEY (id);


--
-- Name: bletch6_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bletch6
    ADD CONSTRAINT bletch6_pkey PRIMARY KEY (id);


--
-- Name: bletch7_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bletch7
    ADD CONSTRAINT bletch7_pkey PRIMARY KEY (id);


--
-- Name: bletch8_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bletch8
    ADD CONSTRAINT bletch8_pkey PRIMARY KEY (id);


--
-- Name: bletch9_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY bletch9
    ADD CONSTRAINT bletch9_pkey PRIMARY KEY (id);


--
-- Name: foo1_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY foo1
    ADD CONSTRAINT foo1_pkey PRIMARY KEY (id);


--
-- Name: foo2_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY foo2
    ADD CONSTRAINT foo2_pkey PRIMARY KEY (id);


--
-- Name: foo3_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY foo3
    ADD CONSTRAINT foo3_pkey PRIMARY KEY (id);


--
-- Name: foo4_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY foo4
    ADD CONSTRAINT foo4_pkey PRIMARY KEY (id);


--
-- Name: foo5_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY foo5
    ADD CONSTRAINT foo5_pkey PRIMARY KEY (id);


--
-- Name: foo6_pkey; Type: CONSTRAINT; Schema: public; Owner: rhaas; Tablespace: 
--

ALTER TABLE ONLY foo6
    ADD CONSTRAINT foo6_pkey PRIMARY KEY (id);


--
-- Name: bar1_bar10_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bar1
    ADD CONSTRAINT bar1_bar10_id_fkey FOREIGN KEY (bar10_id) REFERENCES bar10(id);


--
-- Name: bar1_bar11_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bar1
    ADD CONSTRAINT bar1_bar11_id_fkey FOREIGN KEY (bar11_id) REFERENCES bar11(id);


--
-- Name: bar1_bar12_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bar1
    ADD CONSTRAINT bar1_bar12_id_fkey FOREIGN KEY (bar12_id) REFERENCES bar12(id);


--
-- Name: bar1_bar13_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bar1
    ADD CONSTRAINT bar1_bar13_id_fkey FOREIGN KEY (bar13_id) REFERENCES bar13(id);


--
-- Name: bar1_bar14_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bar1
    ADD CONSTRAINT bar1_bar14_id_fkey FOREIGN KEY (bar14_id) REFERENCES bar14(id);


--
-- Name: bar1_bar2_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bar1
    ADD CONSTRAINT bar1_bar2_id_fkey FOREIGN KEY (bar2_id) REFERENCES bar2(id);


--
-- Name: bar1_bar3_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bar1
    ADD CONSTRAINT bar1_bar3_id_fkey FOREIGN KEY (bar3_id) REFERENCES bar3(id);


--
-- Name: bar1_bar4_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bar1
    ADD CONSTRAINT bar1_bar4_id_fkey FOREIGN KEY (bar4_id) REFERENCES bar4(id);


--
-- Name: bar1_bar5_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bar1
    ADD CONSTRAINT bar1_bar5_id_fkey FOREIGN KEY (bar5_id) REFERENCES bar5(id);


--
-- Name: bar1_bar6_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bar1
    ADD CONSTRAINT bar1_bar6_id_fkey FOREIGN KEY (bar6_id) REFERENCES bar6(id);


--
-- Name: bar1_bar7_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bar1
    ADD CONSTRAINT bar1_bar7_id_fkey FOREIGN KEY (bar7_id) REFERENCES bar7(id);


--
-- Name: bar1_bar8_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bar1
    ADD CONSTRAINT bar1_bar8_id_fkey FOREIGN KEY (bar8_id) REFERENCES bar8(id);


--
-- Name: bar1_bar9_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bar1
    ADD CONSTRAINT bar1_bar9_id_fkey FOREIGN KEY (bar9_id) REFERENCES bar9(id);


--
-- Name: bar1_bletch1_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bar1
    ADD CONSTRAINT bar1_bletch1_id_fkey FOREIGN KEY (bletch1_id) REFERENCES bletch1(id);


--
-- Name: baz1_baz2_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY baz1
    ADD CONSTRAINT baz1_baz2_id_fkey FOREIGN KEY (baz2_id) REFERENCES baz2(id);


--
-- Name: baz1_baz3_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY baz1
    ADD CONSTRAINT baz1_baz3_id_fkey FOREIGN KEY (baz3_id) REFERENCES baz3(id);


--
-- Name: baz1_baz4_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY baz1
    ADD CONSTRAINT baz1_baz4_id_fkey FOREIGN KEY (baz4_id) REFERENCES baz4(id);


--
-- Name: baz1_baz5_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY baz1
    ADD CONSTRAINT baz1_baz5_id_fkey FOREIGN KEY (baz5_id) REFERENCES baz5(id);


--
-- Name: baz1_baz6_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY baz1
    ADD CONSTRAINT baz1_baz6_id_fkey FOREIGN KEY (baz6_id) REFERENCES baz6(id);


--
-- Name: baz1_baz7_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY baz1
    ADD CONSTRAINT baz1_baz7_id_fkey FOREIGN KEY (baz7_id) REFERENCES baz7(id);


--
-- Name: baz1_baz8_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY baz1
    ADD CONSTRAINT baz1_baz8_id_fkey FOREIGN KEY (baz8_id) REFERENCES baz8(id);


--
-- Name: baz1_baz9_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY baz1
    ADD CONSTRAINT baz1_baz9_id_fkey FOREIGN KEY (baz9_id) REFERENCES baz9(id);


--
-- Name: bletch1_bletch2_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bletch1
    ADD CONSTRAINT bletch1_bletch2_id_fkey FOREIGN KEY (bletch2_id) REFERENCES bletch2(id);


--
-- Name: bletch1_bletch3_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bletch1
    ADD CONSTRAINT bletch1_bletch3_id_fkey FOREIGN KEY (bletch3_id) REFERENCES bletch3(id);


--
-- Name: bletch1_bletch4_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bletch1
    ADD CONSTRAINT bletch1_bletch4_id_fkey FOREIGN KEY (bletch4_id) REFERENCES bletch4(id);


--
-- Name: bletch1_bletch5_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bletch1
    ADD CONSTRAINT bletch1_bletch5_id_fkey FOREIGN KEY (bletch5_id) REFERENCES bletch5(id);


--
-- Name: bletch1_bletch6_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bletch1
    ADD CONSTRAINT bletch1_bletch6_id_fkey FOREIGN KEY (bletch6_id) REFERENCES bletch6(id);


--
-- Name: bletch1_bletch7_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bletch1
    ADD CONSTRAINT bletch1_bletch7_id_fkey FOREIGN KEY (bletch7_id) REFERENCES bletch7(id);


--
-- Name: bletch1_bletch8_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bletch1
    ADD CONSTRAINT bletch1_bletch8_id_fkey FOREIGN KEY (bletch8_id) REFERENCES bletch8(id);


--
-- Name: bletch1_bletch9_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY bletch1
    ADD CONSTRAINT bletch1_bletch9_id_fkey FOREIGN KEY (bletch9_id) REFERENCES bletch9(id);


--
-- Name: foo1_bar1_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY foo1
    ADD CONSTRAINT foo1_bar1_id_fkey FOREIGN KEY (bar1_id) REFERENCES bar1(id);


--
-- Name: foo1_baz1_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY foo1
    ADD CONSTRAINT foo1_baz1_id_fkey FOREIGN KEY (baz1_id) REFERENCES baz1(id);


--
-- Name: foo1_foo2_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY foo1
    ADD CONSTRAINT foo1_foo2_id_fkey FOREIGN KEY (foo2_id) REFERENCES foo2(id);


--
-- Name: foo1_foo3_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY foo1
    ADD CONSTRAINT foo1_foo3_id_fkey FOREIGN KEY (foo3_id) REFERENCES foo3(id);


--
-- Name: foo1_foo4_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY foo1
    ADD CONSTRAINT foo1_foo4_id_fkey FOREIGN KEY (foo4_id) REFERENCES foo4(id);


--
-- Name: foo1_foo5_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY foo1
    ADD CONSTRAINT foo1_foo5_id_fkey FOREIGN KEY (foo5_id) REFERENCES foo5(id);


--
-- Name: foo1_foo6_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: rhaas
--

ALTER TABLE ONLY foo1
    ADD CONSTRAINT foo1_foo6_id_fkey FOREIGN KEY (foo6_id) REFERENCES foo6(id);


--
-- PostgreSQL database dump complete
--

