/*
 * Machine driver for EVAL-ADAU1701MINIZ on Analog Devices bfin
 * evaluation boards.
 *
 * Copyright 2011 Analog Devices Inc.
 * Author: Lars-Peter Clausen <lars@metafoo.de>
 *
 * Licensed under the GPL-2 or later.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>

#include "../codecs/adau1701.h"

static const struct snd_soc_dapm_widget snd_rpi_dsproto_dsp_dapm_widgets[] = {
	SND_SOC_DAPM_SPK("Speaker", NULL),
	SND_SOC_DAPM_LINE("Line Out", NULL),
	SND_SOC_DAPM_LINE("Line In", NULL),
};

static const struct snd_soc_dapm_route snd_rpi_dsproto_dsp_dapm_routes[] = {
	{ "Speaker", NULL, "OUT0" },
	{ "Speaker", NULL, "OUT1" },
	{ "Line Out", NULL, "OUT2" },
	{ "Line Out", NULL, "OUT3" },

	{ "IN0", NULL, "Line In" },
	{ "IN1", NULL, "Line In" },
};

static int snd_rpi_dsproto_dsp_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	int ret;

	ret = snd_soc_dai_set_sysclk(codec_dai, ADAU1701_CLK_SRC_OSC, 12288000,
			SND_SOC_CLOCK_IN);

	return ret;
}

static struct snd_soc_ops snd_rpi_dsproto_dsp_ops = {
	.hw_params = snd_rpi_dsproto_dsp_hw_params,
};

#define SND_RPI_DSPROTO_DSP_DAI_FMT (SND_SOC_DAIFMT_I2S | \
				SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBM_CFM)

static struct snd_soc_dai_link snd_rpi_dsproto_dsp_dai[] = {
	{
		.name = "DSProto DSP",
		.stream_name = "DSProto DSP Fi",
		.cpu_dai_name = "bcm2708-i2s.0",
		.codec_dai_name = "adau1701",
		.platform_name = "bcm2708-i2s.0",
		.codec_name = "adau1701.1-0034",
		.ops = &snd_rpi_dsproto_dsp_ops,
		.dai_fmt = SND_RPI_DSPROTO_DSP_DAI_FMT,
	},
};

static struct snd_soc_card snd_rpi_dsproto_dsp = {
	.name = "snd_rpi_dsproto_dsp",
	.owner = THIS_MODULE,
	.dai_link = snd_rpi_dsproto_dsp_dai,
	.num_links = ARRAY_SIZE(snd_rpi_dsproto_dsp_dai),

	.dapm_widgets		= snd_rpi_dsproto_dsp_dapm_widgets,
	.num_dapm_widgets	= ARRAY_SIZE(snd_rpi_dsproto_dsp_dapm_widgets),
	.dapm_routes		= snd_rpi_dsproto_dsp_dapm_routes,
	.num_dapm_routes	= ARRAY_SIZE(snd_rpi_dsproto_dsp_dapm_routes),
};

static int snd_rpi_dsproto_dsp_probe(struct platform_device *pdev)
{

	int ret = 0;

	snd_rpi_dsproto_dsp.dev = &pdev->dev;
	if (pdev->dev.of_node) {
		struct device_node *i2s_node;
		struct snd_soc_dai_link *dai;

		dai = &snd_rpi_dsproto_dsp_dai[0];
		i2s_node = of_parse_phandle(pdev->dev.of_node,
			"i2s-controller", 0);

		if (i2s_node) {
			dai->cpu_dai_name = NULL;
			dai->cpu_of_node = i2s_node;
			dai->platform_name = NULL;
			dai->platform_of_node = i2s_node;
		}

    }
	ret = snd_soc_register_card(&snd_rpi_dsproto_dsp);
	if (ret)
		dev_err(&pdev->dev,
			"snd_soc_register_card() failed: %d\n", ret);

	return ret;


/*
    int ret = 0;
	struct snd_soc_card *card = &snd_rpi_dsproto_dsp;

	card->dev = &pdev->dev;

	ret = devm_snd_soc_register_card(&pdev->dev, &snd_rpi_dsproto_dsp);
    return ret;
*/
}

static int snd_rpi_dsproto_dsp_remove(struct platform_device *pdev)
{
	return snd_soc_unregister_card(&snd_rpi_dsproto_dsp);
}

static const struct of_device_id snd_rpi_dsproto_dsp_of_match[] = {
	{ .compatible = "dsproto,dsproto-dsp", },
	{},
};
MODULE_DEVICE_TABLE(of, snd_rpi_dsproto_dsp_of_match);

static struct platform_driver snd_rpi_dsproto_dsp_driver = {
	.driver = {
		.name = "snd-rpi-dsproto-dsp",
		.pm = &snd_soc_pm_ops,
        .of_match_table = snd_rpi_dsproto_dsp_of_match,
	},
	.probe = snd_rpi_dsproto_dsp_probe,
    .remove = snd_rpi_dsproto_dsp_remove,
};

module_platform_driver(snd_rpi_dsproto_dsp_driver);

MODULE_AUTHOR("Lars-Peter Clausen <lars@metafoo.de>");
MODULE_DESCRIPTION("ALSA SoC bfin ADAU1701 driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:dsproto");
