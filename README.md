Coupled Human and Natural Geospatial Environments (CHANGE)

The Coupled Human and Natural Geospatial Environments (CHANGE) model is a spatially-explicit stochastic computer model designed to simulate regional land change dynamics resulting from multiple human and natural processes and their interactions. These processes include land use transitions such clearing of forests for agriculture or development; land management practices such as timber harvesting and prescribed burning, natural disturbances such as wildfires or insect outbreaks; and natural vegetation succession arising from plant population- and community-level processes. The model’s intended uses include producing scenario-based projections of land cover change at regional to continental levels and conducting simulation experiments to explore how interactions between human activities and natural processes influence future pathways of land cover and land use change.

The algorithms implemented in CHANGE were derived from two existing land change models. The FORE-SCE model was developed as a tool for projecting spatial patterns of land cover and land use at an ecoregion level. Land cover is represented as a relatively small number of broad cover types, such as development, evergreen forest, or cropland. The demand-allocation approach used by FORE-SCE is driven by demand inputs that specify the total area of different land-cover transitions that will occur within a given area in future time steps. The spatial allocation of these land cover changes is then governed by a set of rules and probability surfaces that constrain the geographic patterns of change. 

The LADS model was developed to model natural disturbances and vegetation dynamics across forest-dominated landscapes. Vegetation is represented as discrete successional stages with different structural and floristic characteristics. Between disturbances, vegetation follows predetermined successional pathways with transitions occurring at specified time intervals. Disturbances can initiate an immediate change to a new successional stage, with the transition dependent upon the pre-disturbance successional stage, the intensity of the disturbance, and site characteristics. However, LADS does not include any capabilities for modeling other types of human-driven LCLU transition and considers any non-natural vegetation to belong to a static “background” class that is not simulated.

CHANGE combines the capabilities of FORE-SCE and LADS in an integrated modeling framework. A demand allocation algorithm is used to model transitions among broad LCLU classes, and a state-and-transition model is then applied to simulate more detailed successional dynamics within the subset of naturally vegetation LCLU classes. In addition, a novel human and natural interface (HNI) model has been implemented to project hybrid classes that combine elements of human-dominated and natural land cover types. Wildfires are simulated using a spatially explicit fire spread algorithm that captures key interactions between the human and natural systems, such as the potential for fire spread from human-dominated areas into natural vegetation. The fire simulation algorithms in CHANGE have also been updated to make them more sensitive to climatic variability over time. Vegetation management, including timber harvesting and fuel reduction treatments, is another key interaction between the human and natural systems that is simulated using a modified demand-allocation framework. 

Model output is in the form of raster GIS layers that characterize the spatial patterns of LCLU classes and successional stages at the end of each time step. These files can be processed using standard GIS, image processing and statistical software (e.g., ArcView, ArcGIS, Imagine, R) to analyze spatial patterns of forest vegetation. Various tabular summaries of landscape composition and land cover change transitions are also provided.

The model is a stand-alone program written in Visual C++ environment for use in Microsoft Windows Operating System environment, and in continuous development.

Publications:

1. Michael C. Wimberly, Zhihua Liu, Aashis Lamsal, and Terry L. Sohl. 2014. Simulating forest landscape disturbances as coupled human and natural systems. Chapter 11 In: Modeling Forest Landscape Disturbances. A. H. Perera, B.Sturtevant, and L. J. Buse(Eds.)(Springer, New York, USA, submitted)

2. Zhihua Liu, Michael C. Wimberly, Aashis Lamsal, Terry L. Sohl, Todd J. Hawbaker. 2015. Interactive effects of climate change and wildland-urban interface growth on wildfire risk in the Colorado Front Range Corridor (Landscape Ecology, submitted)

Presentations and proceedings

1. Michael C. Wimberly, Terry L. Sohl, Aashis Lamsal, Zhihua Liu, Todd J.Hawbaker.2013. An Integrated Land Change Model for Projecting Future Climate and Land Change Scenarios. (Oral presentation in Landscape Dynamics Along Environmental Gradients, 2013 Annual Symposium Austin, TX, April 14-18)

2. Aashis Lamsal, Zhihua Liu, Michael C. Wimberly. 2013. Coupling Simulations of Human Driven Land Use Change with Natural Vegetation Dynamics. (Poster presentation in 4thInternational Conference on Computing for Geospatial Research & Application 2013, San Jose, CA, July 22-24)

3. Zhihua Liu, Michael C. Wimberly, Aashis Lamsal, Terry L. Sohl, Todd J. Hawbaker. 2014. Coupled simulation of human-driven and natural land cover change in the Front Range Corridor, CO. International Environmental Modelling and Software Society (iEMSs) 7th International Congress on Environmental Modelling and Software San Diego, California, USA, D.P. Ames, N. Quinn (Eds.)  http://www.iemss.org/society/index.php/iemss-2014-proceedings

4. Aashis Lamsal, Michael C. Wimberly, Zhihua Liu, Terry L. Sohl. 2014. A simulation model of human-natural interactions in dynamic landscapes. International  Environmental Modelling and Software Society (iEMSs) 7th International Congress on Environmental Modelling and Software San Diego, California, USA, D.P. Ames, N. Quinn (Eds.) http://www.iemss.org/society/index.php/iemss-2014-proceedings

5. Zhihua Liu, Michael C. Wimberly, Aashis Lamsal, Terry L. Sohl, Todd J. Hawbaker. 2014. Coupled simulation of land cover change and natural dynamics in Front Range Corridor, Colorado. (Poster 2014 ESA Annual Meeting, Sacramento, CA, August 10-15.)


