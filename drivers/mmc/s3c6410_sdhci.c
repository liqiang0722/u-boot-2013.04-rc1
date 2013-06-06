/*  
 * Samsung S3C6410 SD/MMC driver 
 * Based on sdhci.c
 * Author: LiQiang  E-mail: jxustlq@163.com
 * Licensed under the GPL-2 or later.
 */
 
#include <common.h>
#include <malloc.h>
#include <sdhci.h>
#include <asm/arch/s3c6400.h> 

#ifdef  CONFIG_MMC_CHANNEL
#define MMC_CHANNEL CONFIG_MMC_CHANNEL
#else
#define	MMC_CHANNEL 0
#endif 

#define ELFIN_HSMMC_BASE	0x7C200000
#define MMC_REGS_BASE		(ELFIN_HSMMC_BASE + 0x100000*MMC_CHANNEL)


static void sdhc_set_gpio()
{
	u32 reg;
#if (MMC_CHANNEL == 0)
	reg = readl(GPGCON) & 0xf0000000;
	writel(reg | 0x02222222, GPGCON);

	reg = readl(GPGPUD) & 0xfffff000;
	writel(reg, GPGPUD);
#elif (MMC_CHANNEL == 1)
	writel(0x00222222, GPHCON0);
	writel(0x00000000, GPHCON1);

	reg = readl(GPHPUD) & 0xfffff000;
	writel(reg, GPHPUD);
#else
	printf("#####err: SDMMC channel is not defined!\n");
#endif
}

int s3c_sdhci_init(u32 regbase, u32 max_clk, u32 min_clk, u32 quirks)
{
	struct sdhci_host *host = NULL;
	host = (struct sdhci_host *)malloc(sizeof(struct sdhci_host));
	if (!host) {
		printf("#####err: sdhci_host malloc fails!\n");
		return 1;
	}
	sdhc_set_gpio();
	host->name = "Samsung  Host Controller";
	host->ioaddr = (void *)regbase;
	host->quirks = quirks;


	host->voltages = MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_165_195;
	if (quirks & SDHCI_QUIRK_REG32_RW)
		host->version = sdhci_readl(host, SDHCI_HOST_VERSION - 2) >> 16;
	else
		host->version = sdhci_readw(host, SDHCI_HOST_VERSION);

	host->host_caps = MMC_MODE_HC;

	add_sdhci(host, max_clk, min_clk);
	return 0;
}



int board_mmc_init(bd_t *bis)
{
	return s3c_sdhci_init(MMC_REGS_BASE, 52000000, 400000, 0);
}
