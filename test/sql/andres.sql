load 'saio';

set join_collapse_limit to 100;
set from_collapse_limit to 100;

set saio_seed to 0.5;
set saio_algorithm to recalc;
set saio_equilibrium_factor to 1;
set saio_initial_temperature_factor to 2.0;
set saio_temperature_reduction_factor to 0.4;
set saio_moves_before_frozen to 2;

SET SEARCH_PATH = test_data, test_view;

prepare foo as 
SELECT *
FROM
    proband
    JOIN proband__sample USING(proband_id)
    JOIN sample USING (sample_id)

    JOIN information_generic_allowance USING (proband_id)
    JOIN information_genetic_allowance USING (proband_id)
    JOIN information_patient_diagnosis diag_1 USING (proband_id)
    JOIN information_patient_diagnosis diag_2 USING (proband_id)
    JOIN information_patient_placeholder patient_histology USING (proband_id)

    JOIN information_sample_placeholder sample_rna_experiment_1_status USING (sample_id)
    JOIN information_sample_placeholder sample_rna_experiment_1_data USING (sample_id)

    JOIN information_sample_placeholder sample_rna_experiment_2_status USING (sample_id)
    JOIN information_sample_placeholder sample_rna_experiment_2_data USING (sample_id)

    JOIN information_sample_placeholder sample_rna_experiment_3_status USING (sample_id)
    JOIN information_sample_placeholder sample_rna_experiment_3_data USING (sample_id)

WHERE TRUE
    AND information_generic_allowance.generic_allowance_granted = true
    AND information_genetic_allowance.information_6_granted = true

    AND diag_1.diagnosis_icd10 = 1343 /*some icd code*/
    AND diag_2.diagnosis_icd10 = 1344 /*another icd code*/

    AND (
        SELECT
 	    available_information_nr / available_information_nr
    	FROM
	    information_set_completition_status
	WHERE
	    information_set_completition_status.information_set_instance_id = patient_histology.information_set_instance_id
    ) > 0.8


    AND (
        SELECT
 	    available_information_nr / available_information_nr
    	FROM
	    information_set_completition_status
	WHERE
	    information_set_completition_status.information_set_instance_id = diag_1.information_set_instance_id
    ) > 0.5


    AND (
        SELECT
 	    available_information_nr / available_information_nr
    	FROM
	    information_set_completition_status
	WHERE
	    information_set_completition_status.information_set_instance_id = diag_2.information_set_instance_id
    ) > 0.5;

deallocate foo;
