#!/bin/bash
for i in 2.1 2.2 2.3 2.4 2.5 2.6 2.7 2.8 2.9 3;
do
  sed -e "s/INDEX/$i/g" Signal_pu200_medium_baseline.job.tmpl > Signal_pu200_medium_baseline_Pt_$i.job
  sed -e "s/INDEX/$i/g" bkg_pu200_medium_baseline.job.tmpl > bkg_pu200_medium_baseline_Pt_$i.job
done
~      
